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
		trail->Add("sparrrk");
		particleTrails_.push_back(std::move(trail));
	}
}

void SparkEffect::Trigger(const Vector3& position)
{
	for (auto& t : particleTrails_)
	{
		t->Clear();          // 既存軌跡を消す
		t->Add("sparrrk");
		t->SetEnabled(true); // ついでに有効化
	}

	particle.Trigger("sparrrrk2", position);
	isActive_ = true;
}

void SparkEffect::Update(float dt, const Matrix4x4& vpMatrix)
{
	particle.Update(dt, vpMatrix);

	// 粒ワールド行列
	const auto worlds = particle.GetParticleWorlds("sparrrrk2");
	//const size_t alive = std::min(particle.GetAliveCount("sparrrrk2"), particleTrails_.size());
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

	if (alive == 0)
	{
		particle.Stop("sparrrrk2");
		isActive_ = false;
	}
}

void SparkEffect::Draw(CmdObj* cmdObj)
{
	if (!isActive_) return;

	for (auto& trail : particleTrails_)
	{
		trail->Draw(cmdObj);
	}
}