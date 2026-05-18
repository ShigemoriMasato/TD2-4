#include "ParticleDrawer.h"
#include <algorithm>

void ParticleDrawer::Initialize(
	SHEngine::DrawDataManager* drawDataManager,
	SHEngine::ModelManager* modelManager,
	const Config& cfg)
{
	// ポインタ取得
	drawDataManager_ = drawDataManager;
	modelManager_ = modelManager;

	// config設定
	SetConfig(cfg);

	Clear();
}

void ParticleDrawer::SetConfig(const Config& cfg)
{
	config_ = cfg;
	config_.maxModels = std::max(1u, config_.maxModels);
	config_.maxInstancesPerModel = std::max(1u, config_.maxInstancesPerModel);
}

void ParticleDrawer::Clear()
{
	particles_.clear();
}

void ParticleDrawer::Register(IParticle* particle)
{
	if (!particle) return;
	particles_.push_back(particle);
}

void ParticleDrawer::Draw(CmdObj* cmdObj, const Matrix4x4& vpMatrix)
{
	if (!cmdObj) return;
	if (!drawDataManager_ || !modelManager_) return;

	// 収集（モデル単位に詰める）
	for (IParticle* p : particles_)
	{
		if (!p) continue;

		// pに描画データは何個あるか
		const uint32_t count = p->GetGpuInstanceCount();
		if (count == 0) continue;

		// モデルハンドルを取得
		const int modelHandle = p->GetModelHandle();
		if (modelHandle < 0) continue;

		// 既にあれば取得。なければ作成してから取得。そう、このアクセス方法ならね。
		auto& batch = batches_[modelHandle];

		// なかった時はrenderObjectが空っぽだから作成する
		if (!batch.render)
		{
			const auto modelData = modelManager_->GetNodeModelData(modelHandle);
			const auto drawData = drawDataManager_->GetDrawData(modelData.drawDataIndex);

			batch.render = std::make_unique<SHEngine::RenderObject>("ParticleDrawerBatch");
			batch.render->Initialize();
			batch.render->SetDrawData(drawData);

			batch.render->psoConfig_.vs = "Particle/Particle.VS.hlsl";
			batch.render->psoConfig_.ps = "Particle/Particle.PS.hlsl";
			batch.render->psoConfig_.blendID = SHEngine::PSO::BlendStateID::Add;
			batch.render->psoConfig_.depthStencilID = SHEngine::PSO::DepthStencilID::Default;
			batch.render->psoConfig_.rasterizerID = SHEngine::PSO::RasterizerID::Fill;
			batch.render->SetUseTexture(true);

			batch.render->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "VP");
			batch.render->CreateSRV(sizeof(IParticle::InstanceGpu), config_.maxInstancesPerModel, ShaderType::VERTEX_SHADER, "ParticleInstances");
			batch.render->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "Color");

			/// 各モデルは最大でconfig_.maxInstancesPerModel個のインスタンスを描画できるようにする。
			/// モデルAが一個しか描画されてなくてもモデルBが大量に描画されてたらモデルAも大量にreserveする
			/// いずれ改良もあり
			batch.instances.reserve(config_.maxInstancesPerModel);
		}

		// GPU転送用のデータを取得
		const IParticle::InstanceGpu* src = p->GetGpuInstanceData();

		// remain = 残りの描画可能数 = (max描画数 - すでに描画予定リストに入ってる数)
		const uint32_t remain =
			(batch.instances.size() >= config_.maxInstancesPerModel)
			? 0u
			: (config_.maxInstancesPerModel - static_cast<uint32_t>(batch.instances.size()));

		// toCopy = 今回描画予定リストに入れる数 = min(描画データの数, remain)
		const uint32_t toCopy = std::min<uint32_t>(count, remain);

		// 描画予定リストに追加する余地があればその分だけ追加する
		if (toCopy > 0)
		{
			batch.instances.insert(batch.instances.end(), src, src + toCopy);
		}
	}

	// 描画（モデルごとに1 draw）
	for (auto& [modelHandle, batch] : batches_)
	{
		if (!batch.render) continue;
		if (batch.instances.empty()) continue;

		batch.render->CopyBufferData(0, &vpMatrix, sizeof(Matrix4x4));
		batch.render->CopyBufferData(1, batch.instances.data(), sizeof(IParticle::InstanceGpu) * batch.instances.size());

		const Vector4 white = { 1,1,1,1 };
		batch.render->CopyBufferData(2, &white, sizeof(Vector4));

		batch.render->instanceNum_ = static_cast<uint32_t>(batch.instances.size());
		batch.render->Draw(cmdObj);

		batch.instances.clear();
	}

	// Register() で積まれた Particle* はフレーム単位の一時リストとして扱う。
	// これをクリアしないと、次フレーム以降に「既に破棄された Particle へのダングリングポインタ」
	// が残り、modelHandle が破壊されて ModelManager::GetNodeModelData の assert に繋がる。		
	particles_.clear();
}