#include "TrailDrawer.h"
#include <GameObject/Effect/Trail/ITrail.h>
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
	// VS: b0 VPBuffer
	render_->CreateSRV(sizeof(BatchVertex), uint32_t(maxVertexCountTotal_), ShaderType::VERTEX_SHADER, "TrailBatchVertices");
	render_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "VP");
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

void TrailDrawer::Register(ITrail* trail)
{
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
	const int perTrail = maxVertexCountPerTrail_;
	const int maxTrails = config_.maxTrails;

	int slot = 0;
	for (ITrail* t : trails_)
	{
		// 無効化されているトレイルはcontinue
		if (!t || !t->GetIsActive()) continue;
		// config_.maxTrailsを超えた時はBreak
		if (slot >= maxTrails) break;

		// そのトレイルの頂点数取得。0以上maxVertexCountPerTrail_未満にclamp
		const int vcount = std::clamp(static_cast<int>(t->GetGpuVertices().size()), 0, perTrail);
		const int dstBase = slot * perTrail;
		if (vcount > 0)
		{
			const auto& src = t->GetGpuVertices();

			if (dstBase + vcount > maxVertexCountTotal_)
			{
				assert(false);
				break;
			}
			//// BuildVertices() 内、memcpy の直前に追加
			//for (int j = 0; j < vcount; ++j)
			//{
			//	if (src[j].position.w == 0.0f)
			//	{
			//		// ログ出力（エンジンのログ関数に合わせて）
			//		logger_->error("TrailDrawer: found w==0 in trail slot {} vertex {} (id?)", slot, j);
			//		// 一時的に修正して続行する（デバッグ用）
			//		// const_cast を使うのは一時的なデバッグのみ推奨
			//		// const_cast<GpuVertex&>(src[j]).position.w = 1.0f;
			//	}
			//}
			std::memcpy(&batchVertices_[dstBase], src.data(), static_cast<size_t>(vcount) * sizeof(BatchVertex));
		}

		for (int i = dstBase + vcount; i < dstBase + perTrail; ++i)
		{
			batchVertices_[i].position.w = 0.0f;
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

	// VS: t0 頂点配列
	render_->CopyBufferData(0, batchVertices_.data(), sizeof(BatchVertex) * batchVertices_.size());
	// VS: b0 VPBuffer
	render_->CopyBufferData(1, &vpMatrix, sizeof(Matrix4x4));

	render_->instanceNum_ = 1;
	render_->Draw(cmdObj);

	trails_.clear();
}
