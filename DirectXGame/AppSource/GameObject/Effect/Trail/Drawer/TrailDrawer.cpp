#include "TrailDrawer.h"
#include <GameObject/Effect/Trail/Trail.h>
#include <algorithm>

void TrailDrawer::Initialize(SHEngine::DrawDataManager* drawDataManager, const Config& cfg)
{
	// ポインタ取得
	drawDataManager_ = drawDataManager;

	// config設定
	SetConfig(cfg);

	// 現状のconfigから考えた１トレイルあたりの最大頂点数
	maxVertexCountPerTrail_ = (std::max(1, config_.maxSegmentsPerTrail) + 1) * 2;
	// 現状のconfigから考えた全トレイルの最大合計頂点数
	maxVertexCountTotal_ = std::max(1, config_.maxTrails) * maxVertexCountPerTrail_;

	// 頂点配列リサイズ & 初期化
	batchVertices_.resize(static_cast<size_t>(maxVertexCountTotal_));
	std::fill(batchVertices_.begin(), batchVertices_.end(), BatchVertex{});

	// ダミーVB + 固定IB
	std::vector<VertexData> dummyVB(static_cast<size_t>(maxVertexCountTotal_));
	for (auto& v : dummyVB)
	{
		v.position = Vector4(0, 0, 0, 1);
		v.texcoord = Vector2(0, 0);
		v.normal = Vector3(0, 1, 0);
	}
	drawDataManager_->AddVertexBuffer(dummyVB);
	BuildIndexBuffer();

	drawDataIndex_ = drawDataManager_->CreateDrawData();
	auto drawData = drawDataManager_->GetDrawData(drawDataIndex_);

	// RenderObject作成
	render_ = std::make_unique<SHEngine::RenderObject>("TrailDrawer");
	render_->Initialize();
	render_->SetDrawData(drawData);
	render_->psoConfig_.vs = "Trail/SwordTrail.VS.hlsl";
	render_->psoConfig_.ps = "Trail/SwordTrail.PS.hlsl";
	render_->psoConfig_.blendID = SHEngine::PSO::BlendStateID::Add;
	render_->psoConfig_.depthStencilID = SHEngine::PSO::DepthStencilID::Default;
	render_->psoConfig_.rasterizerID = SHEngine::PSO::RasterizerID::CullNone;
	render_->SetUseTexture(true);

	// VS: t0 頂点配列
	// VS: b0 viewProjection行列
	// PS: b0 色
	// PS: b1 テクスチャインデック
	render_->CreateSRV(sizeof(BatchVertex), uint32_t(maxVertexCountTotal_), ShaderType::VERTEX_SHADER, "TrailBatchVertices");
	render_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "VP");
	render_->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "Color");
	render_->CreateCBV(sizeof(uint32_t), ShaderType::PIXEL_SHADER, "TextureIndex");
}

void TrailDrawer::SetConfig(const Config& cfg)
{
	config_ = cfg;
	config_.maxTrails = std::max(1, config_.maxTrails);
	config_.maxSegmentsPerTrail = std::max(1, config_.maxSegmentsPerTrail);
}

void TrailDrawer::Clear()
{
	trails_.clear();
}

void TrailDrawer::Register(Trail* trail)
{
	if (!trail) return;
	trails_.push_back(trail);
}

void TrailDrawer::BuildIndexBuffer()
{
	const int maxSamplesPerTrail = maxVertexCountPerTrail_ / 2;
	const int maxQuadsPerTrail = maxSamplesPerTrail - 1;

	std::vector<uint32_t> indices;
	indices.reserve(static_cast<size_t>(config_.maxTrails) * static_cast<size_t>(maxQuadsPerTrail) * 6);

	for (int t = 0; t < config_.maxTrails; ++t)
	{
		const uint32_t baseV = uint32_t(t * maxVertexCountPerTrail_);
		for (int i = 0; i < maxQuadsPerTrail; ++i)
		{
			const uint32_t i0 = baseV + uint32_t(i * 2 + 0);
			const uint32_t i1 = baseV + uint32_t(i * 2 + 1);
			const uint32_t i2 = baseV + uint32_t((i + 1) * 2 + 0);
			const uint32_t i3 = baseV + uint32_t((i + 1) * 2 + 1);

			// (i0, i1, i2) (i2, i1, i3)
			indices.push_back(i0);
			indices.push_back(i1);
			indices.push_back(i2);

			indices.push_back(i2);
			indices.push_back(i1);
			indices.push_back(i3);
		}
	}

	drawDataManager_->AddIndexBuffer(std::move(indices));
}

void TrailDrawer::BuildVertices()
{
	// 頂点配列初期化
	//std::fill(batchVertices_.begin(), batchVertices_.end(), BatchVertex{});

	// memset方式に変更
	std::memset(batchVertices_.data(), 0, sizeof(BatchVertex) * batchVertices_.size());



	const int perTrail = maxVertexCountPerTrail_;
	const int maxTrails = config_.maxTrails;

	int slot = 0;
	for (Trail* t : trails_)
	{
		// 無効化されているトレイルはcontinue
		if (!t || !t->IsEmitting()) continue;
		// config_.maxTrailsを超えた時はBreak
		if (slot >= maxTrails) break;

		// そのトレイルの頂点数取得。0以上maxVertexCountPerTrail_未満にclamp
		const int vcount = std::clamp(t->GetActiveVertexCount(), 0, perTrail);
		if (vcount > 0)
		{
			const auto& src = t->GetGpuVertices();
			const int dstBase = slot * perTrail;

			if (dstBase + vcount > maxVertexCountTotal_)
			{
				assert(false);
				break;
			}

			std::memcpy(&batchVertices_[dstBase], src.data(), static_cast<size_t>(vcount) * sizeof(BatchVertex));
		}

		// 残りはゼロ -> PSでclipされる前提
		++slot;
	}
}

void TrailDrawer::Draw(CmdObj* cmdObj, const Matrix4x4& vpMatrix)
{
	if (!cmdObj) return;
	if (!render_) return;

	BuildVertices();

	render_->CopyBufferData(0, batchVertices_.data(), sizeof(BatchVertex) * batchVertices_.size());
	render_->CopyBufferData(1, &vpMatrix, sizeof(Matrix4x4));

	// PSのcolorは頂点色で制御するので白固定
	const Vector4 white = { 1,1,1,1 };
	render_->CopyBufferData(2, &white, sizeof(Vector4));

	const uint32_t textureIndex = 0; // 今はテクスチャ1個固定の前提
	render_->CopyBufferData(3, &textureIndex, sizeof(uint32_t));


	render_->instanceNum_ = 1;
	render_->Draw(cmdObj);

	trails_.clear();
}
