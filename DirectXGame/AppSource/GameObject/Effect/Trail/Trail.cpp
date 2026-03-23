#include "Trail.h"
#include <algorithm>
#include <cmath>

using namespace SHEngine;

namespace
{
	float DistanceVec3(const Vector3& a, const Vector3& b)
	{
		const float dx = a.x - b.x;
		const float dy = a.y - b.y;
		const float dz = a.z - b.z;
		return std::sqrt(dx * dx + dy * dy + dz * dz);
	}
}

void Trail::Initialize(DrawDataManager* drawDataManager, TextureManager* textureManager, const Config& config)
{
	drawDataManager_ = drawDataManager;
	textureManager_ = textureManager;
	config_ = config;

	// 分割数を1以上に
	config_.maxSegments = std::max(1, config_.maxSegments);
	// 寿命を0.001秒以上に
	config_.lifeTime = std::max(0.001f, config_.lifeTime);
	// 最小距離を0以上に
	config_.minDistance = std::max(0.0f, config_.minDistance);

	// (分割数 + 1) * 2点分の頂点を用意
	maxVertexCount_ = (config_.maxSegments + 1) * 2;
	// リサイズ
	gpuVertices_.resize(maxVertexCount_);
	// 初期化
	std::fill(gpuVertices_.begin(), gpuVertices_.end(), GpuVertex{});

	// InputLayoutに合わせたダミーのDrawDataを作成
	if (dummyDrawDataIndex_ == -1)
	{
		std::vector<VertexData> dummyVertices(maxVertexCount_);
		for (auto& v : dummyVertices)
		{
			v.position = Vector4(0, 0, 0, 1);
			v.texcoord = Vector2(0, 0);
			v.normal = Vector3(0, 1, 0);
		}

		// インデックスは「2点×(N-1)区間」→ quad×2tri → 6*(N-1)
		const int maxSamples = config_.maxSegments + 1;
		const int maxQuads = maxSamples - 1;
		std::vector<uint32_t> indices;
		indices.reserve(maxQuads * 6);

		for (int i = 0; i < maxQuads; ++i)
		{
			const uint32_t i0 = uint32_t(i * 2 + 0);
			const uint32_t i1 = uint32_t(i * 2 + 1);
			const uint32_t i2 = uint32_t((i + 1) * 2 + 0);
			const uint32_t i3 = uint32_t((i + 1) * 2 + 1);

			// (i0, i1, i2) (i2, i1, i3)
			indices.push_back(i0);
			indices.push_back(i1);
			indices.push_back(i2);

			indices.push_back(i2);
			indices.push_back(i1);
			indices.push_back(i3);
		}

		drawDataManager_->AddVertexBuffer(dummyVertices);
		drawDataManager_->AddIndexBuffer(indices);
		dummyDrawDataIndex_ = drawDataManager_->CreateDrawData();
	}

	auto drawData = drawDataManager_->GetDrawData(dummyDrawDataIndex_);

	// テクスチャ
	SetTexture(config_.texturePath);

	// RenderObject（Normal）
	renderNormal_ = std::make_unique<RenderObject>("Trail_Normal");
	renderNormal_->Initialize();
	renderNormal_->SetDrawData(drawData);
	renderNormal_->psoConfig_.vs = "Trail/SwordTrail.VS.hlsl";
	renderNormal_->psoConfig_.ps = "Trail/SwordTrail.PS.hlsl";
	renderNormal_->psoConfig_.blendID = PSO::BlendStateID::Normal;
	renderNormal_->psoConfig_.depthStencilID = PSO::DepthStencilID::Default;
	renderNormal_->psoConfig_.rasterizerID = PSO::RasterizerID::CullNone;
	renderNormal_->SetUseTexture(true);

	// RenderObject（Add）
	renderAdd_ = std::make_unique<RenderObject>("Trail_Add");
	renderAdd_->Initialize();
	renderAdd_->SetDrawData(drawData);
	renderAdd_->psoConfig_.vs = "Trail/SwordTrail.VS.hlsl";
	renderAdd_->psoConfig_.ps = "Trail/SwordTrail.PS.hlsl";
	renderAdd_->psoConfig_.blendID = PSO::BlendStateID::Add;
	renderAdd_->psoConfig_.depthStencilID = PSO::DepthStencilID::Default;
	renderAdd_->psoConfig_.rasterizerID = PSO::RasterizerID::CullNone;
	renderAdd_->SetUseTexture(true);

	// === バッファ構成 ===
	// VS: t0 = vertices
	// VS: b0 = vp
	// PS: b0 = color
	// PS: b1 = textureIndex
	srvVertexIndex_ = renderNormal_->CreateSRV(sizeof(GpuVertex), uint32_t(maxVertexCount_), ShaderType::VERTEX_SHADER, "TrailVertices");
	renderAdd_->CreateSRV(sizeof(GpuVertex), uint32_t(maxVertexCount_), ShaderType::VERTEX_SHADER, "TrailVertices");
	cbvVpIndex_ = renderNormal_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "VP");
	renderAdd_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "VP");
	cbvColorIndex_ = renderNormal_->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "Color");
	renderAdd_->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "Color");
	cbvTextureIndex_ = renderNormal_->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");
	renderAdd_->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");

	Clear();
}

void Trail::Clear()
{
	samples_.clear();
	hasLast_ = false;

	std::fill(gpuVertices_.begin(), gpuVertices_.end(), GpuVertex{});
	if (renderNormal_)
	{
		renderNormal_->CopyBufferData(srvVertexIndex_, gpuVertices_.data(), sizeof(GpuVertex) * gpuVertices_.size());
		renderNormal_->instanceNum_ = 0;
	}
	if (renderAdd_)
	{
		renderAdd_->CopyBufferData(srvVertexIndex_, gpuVertices_.data(), sizeof(GpuVertex) * gpuVertices_.size());
		renderAdd_->instanceNum_ = 0;
	}
}

void Trail::SetTexture(const std::string& texturePath)
{
	textureHandle_ = textureManager_->LoadTexture(texturePath);
}


void Trail::PushSegment(const Vector3& baseWS, const Vector3& tipWS)
{
	if (!enabled_) return;

	// 間引き（どちらかが一定以上動いたら追加）
	if (hasLast_)
	{
		const float db = DistanceVec3(baseWS, lastBase_);
		const float dt = DistanceVec3(tipWS, lastTip_);
		if (std::max(db, dt) < config_.minDistance)
		{
			return;
		}
	}

	lastBase_ = baseWS;
	lastTip_ = tipWS;
	hasLast_ = true;

	Sample s;
	s.base = baseWS;
	s.tip = tipWS;
	s.age = 0.0f;
	samples_.push_back(s);

	// 最大サンプル数 = maxSegments + 1
	const int maxSamples = config_.maxSegments + 1;
	while (int(samples_.size()) > maxSamples)
	{
		samples_.pop_front();
	}
}

void Trail::Update(float deltaTime, const Matrix4x4& vpMatrix)
{
	if (!enabled_) return;

	// age更新 + 寿命を超えたものを削除
	for (auto& s : samples_)
	{
		s.age += deltaTime;
	}
	while (!samples_.empty() && samples_.front().age >= config_.lifeTime)
	{
		samples_.pop_front();
	}
	RebuildVertices(vpMatrix);
}

void Trail::RebuildVertices(const Matrix4x4& vpMatrix)
{
	// サンプルが2未満なら描画しない
	if (samples_.size() < 2)
	{
		renderNormal_->instanceNum_ = 0;
		renderAdd_->instanceNum_ = 0;
		return;
	}

	// Uを0..1で割り当て（簡易：インデックス基準）
	const int n = int(samples_.size());
	for (int i = 0; i < n; ++i)
	{
		const float u = (n <= 1) ? 0.0f : (float(i) / float(n - 1));
		samples_[i].u = u;
	}

	// 頂点組み立て（base/tipの2頂点×サンプル数）
	const int vertexCount = std::min(n * 2, maxVertexCount_);
	for (int i = 0; i < vertexCount / 2; ++i)
	{
		const auto& s = samples_[i];

		// フェード（古いほど透明）
		const float t = std::clamp(1.0f - (s.age / config_.lifeTime), 0.0f, 1.0f);

		auto makeV = [&](const Vector3& p, float v) -> GpuVertex
			{
				GpuVertex out{};
				out.position = Vector4(p, 1.0f);
				out.uv = Vector2(s.u, v);
				out.normal = Vector3(0.0f, 1.0f, 0.0f); // ライト計算しない前提でダミー
				out.color = Vector4(1, 1, 1, t);
				return out;
			};

		gpuVertices_[i * 2 + 0] = makeV(s.base, 0.0f);
		gpuVertices_[i * 2 + 1] = makeV(s.tip, 1.0f);
	}

	// 残りはクリア（前フレームの残骸防止）
	for (int i = vertexCount; i < maxVertexCount_; ++i)
	{
		gpuVertices_[i] = GpuVertex{};
	}

	// GPUへ転送
	renderNormal_->CopyBufferData(srvVertexIndex_, gpuVertices_.data(), sizeof(GpuVertex) * gpuVertices_.size());
	renderAdd_->CopyBufferData(srvVertexIndex_, gpuVertices_.data(), sizeof(GpuVertex) * gpuVertices_.size());

	// VP、色、テクスチャ
	renderNormal_->CopyBufferData(cbvVpIndex_, &vpMatrix, sizeof(Matrix4x4));
	renderAdd_->CopyBufferData(cbvVpIndex_, &vpMatrix, sizeof(Matrix4x4));

	renderNormal_->CopyBufferData(cbvColorIndex_, &config_.colorNormal, sizeof(Vector4));
	renderAdd_->CopyBufferData(cbvColorIndex_, &config_.colorAdd, sizeof(Vector4));

	renderNormal_->CopyBufferData(cbvTextureIndex_, &textureHandle_, sizeof(int));
	renderAdd_->CopyBufferData(cbvTextureIndex_, &textureHandle_, sizeof(int));

	// 描画する index 数（quad数 = (サンプル数-1)）
	const int quadCount = int(samples_.size()) - 1;
	// RenderObjectは「instanceNum_」をインスタンス数として使うが、ここではインスタンスではなく
	// ただ描画をONにするため 1 を入れる（実際の描画頂点数はIBで決まる）。
	// ただし「最大IB」分描いてしまうので、IBを可変にできない現状では「不要部分は透明にする」戦略を取る。
	// → そのため、古い/未使用頂点はalpha=0にして消す。
	(void)quadCount;

	renderNormal_->instanceNum_ = 1;
	renderAdd_->instanceNum_ = 1;
}

void Trail::Draw(CmdObj* cmdObj)
{
	if (!enabled_) return;
	if (!renderNormal_ || !renderAdd_) return;


	if (config_.drawNormal && renderNormal_->instanceNum_ > 0)
	{
		renderNormal_->Draw(cmdObj);
	}
	if (config_.drawAdd && renderAdd_->instanceNum_ > 0)
	{
		renderAdd_->Draw(cmdObj);
	}
}