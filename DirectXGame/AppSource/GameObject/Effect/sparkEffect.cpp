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
	// 粒ごとのトレイル（最大数分を先に確保・生成）
	for (size_t i = 0; i < kMaxParticleTrails_; i++)
	{
		auto trail = std::make_unique<MultiTrail>();
		trail->Initialize(drawDataManager, textureManager, trailPresetDataBank);
		trail->Add("testTrail2");
		particleTrails_.push_back(std::move(trail));
	}
}

void SparkEffect::Trigger(const Vector3& position)
{
	particle.Trigger("sparrrrk2", position);
}

void SparkEffect::Update(float dt, const Matrix4x4& vpMatrix)
{
	particle.Update(dt, vpMatrix);

	// 粒ワールド行列
	const auto worlds = particle.GetParticleWorlds("sparrrrk2");
	const size_t alive = std::min(worlds.size(), particleTrails_.size());

	// 粒が存在する分だけ、トレイルを粒に追従させる
	for (size_t i = 0; i < alive; ++i)
	{
		particleTrails_[i]->SetModelWorld(worlds[i]);
		particleTrails_[i]->SetEnabled(true);
		particleTrails_[i]->Update(dt, vpMatrix);
	}
	// 余ったトレイルは無効化（表示・更新停止）
	for (size_t i = alive; i < particleTrails_.size(); ++i)
	{
		particleTrails_[i]->SetEnabled(false);
	}
}

void SparkEffect::Draw(CmdObj* cmdObj)
{
	particle.Draw(cmdObj);
	for (auto& trail : particleTrails_)
	{
		trail->Draw(cmdObj);
	}
}