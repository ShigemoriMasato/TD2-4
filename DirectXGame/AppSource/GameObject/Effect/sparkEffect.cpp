#include "sparkEffect.h"

void SparkEffect::Initialize(
	SHEngine::TextureManager* textureManager,
	SHEngine::ModelManager* modelManager,
	CommonData* commonData)
{
	particle.Initialize(textureManager, modelManager, commonData);
	particle.Add("death");
	particle.SetEmittingFlag(0, false);
	trails.Initialize(textureManager, commonData);
	for (size_t i = 0; i < 30; ++i) { trails.Add("sparrrk"); }
}

void SparkEffect::Trigger()
{
	// パーティクル起動
	particle.SetEmittingFlag(0, true);
}

void SparkEffect::Update(float dt)
{
	particle.SetModelWorld(modelWorld_);
	particle.Update(dt);

	// 粒ワールド行列
	const auto worlds = particle.GetParticleWorlds(0);

	size_t max = std::min(worlds.size(), static_cast<size_t>(30));

	for (size_t i = 0; i < max; ++i)
	{
		trails.SetModelWorld(int32_t(i), worlds[i]);
	}
	
	trails.Update(dt);
}

void SparkEffect::Draw()
{
	//particle.Draw();
	trails.Draw();
}