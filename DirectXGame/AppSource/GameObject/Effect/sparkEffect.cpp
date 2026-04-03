#include "sparkEffect.h"

void SparkEffect::Initialize(
	SHEngine::DrawDataManager* drawDataManager,
	SHEngine::TextureManager* textureManager,
	SHEngine::ModelManager* modelManager,
	TrailPresetDataBank* trailPresetDataBank,
	ParticlePresetDataBank* particlePresetDataBank)
{
	particle.Initialize(drawDataManager, textureManager, modelManager, particlePresetDataBank);
	particle.Add("sparrrrk2");
	for (size_t i = 0; i < kMaxParticleTrails_; i++)
	{
		auto trail = std::make_unique<MultiTrail>();
		trail->Initialize(drawDataManager, textureManager, trailPresetDataBank);
		trail->Add("sparrrk");
		trail->SetEmittingFlag(false);
		particleTrails_.push_back(std::move(trail));
	}
}

void SparkEffect::Trigger(const Vector3& position)
{
	// パーティクル起動
	particle.SetEmittingFlag(particleIndex, true);

	// トレイル起動
	for (auto& t : particleTrails_)
	{
		t->SetEmittingFlag(true);
	}
}

void SparkEffect::Update(float dt, const Matrix4x4& vpMatrix)
{
	particle.Update(dt, vpMatrix);

	// 粒ワールド行列
	const auto worlds = particle.GetParticleWorlds(particleIndex);
	const size_t alive = std::min(worlds.size(), particleTrails_.size());

	// 粒が存在する分だけ、トレイルを粒に追従させる
	for (size_t i = 0; i < alive; ++i)
	{
		particleTrails_[i]->SetModelWorld(worlds[i]);
		particleTrails_[i]->Update(dt, vpMatrix);
	}
}

void SparkEffect::Draw(CmdObj* cmdObj)
{
	for (auto& trail : particleTrails_)
	{
		trail->Draw(cmdObj);
	}
}