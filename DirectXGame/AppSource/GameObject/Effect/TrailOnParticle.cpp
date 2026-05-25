#include "TrailOnParticle.h"

void TrailOnParticle::Initialize(SHEngine::TextureManager* textureManager, SHEngine::ModelManager* modelManager, CommonData* commonData, std::string particlePresetName, std::string trailPresetName)
{
	particle.Initialize(textureManager, modelManager, commonData);
	particle.Add(particlePresetName);
	particle.SetEmittingFlag(0, false);
	trails.Initialize(textureManager, commonData);
	for (size_t i = 0; i < 30; ++i) { trails.Add(trailPresetName); }
	active_ = false;
}

void TrailOnParticle::Trigger()
{
	// パーティクル起動
	active_ = true;
	particle.SetEmittingFlag(0, true);
	trails.Clear();
}

void TrailOnParticle::Update(float dt)
{
	if (!active_) return;

	particle.SetModelWorld(modelWorld_);
	particle.Update(dt);

	// 粒ワールド行列
	const auto worlds = particle.GetParticleWorlds(0);

	size_t max = std::min(worlds.size(), static_cast<size_t>(30));

	//for (size_t i = 0; i < max; ++i)
	//{
	//	trails.SetModelWorld(int32_t(i), worlds[i]);
	//}
	
	trails.Update(dt);

	if (worlds.empty())
	{
		active_ = false;
		trails.Clear();
	}
}

void TrailOnParticle::Draw()
{
	if (!active_) return;
	//particle.Draw();
	trails.Draw();
}