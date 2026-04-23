#include "ParticleOnTrail.h"

void ParticleOnTrail::Initialize(SHEngine::TextureManager* textureManager, SHEngine::ModelManager* modelManager, CommonData* commonData, std::string particlePresetName, std::string trailPresetName)
{
	particle.Initialize(textureManager, modelManager, commonData);
	particle.Add(particlePresetName);
	particle.SetEmittingFlag(0, false);
	trails.Initialize(textureManager, commonData);
	for (size_t i = 0; i < 30; ++i)
	{
		trails.Add(trailPresetName); 
		trails.SetEmittingFlag(int32_t(i), false);
	}
	active_ = false;
}

void ParticleOnTrail::Trigger()
{
	// パーティクル起動
	active_ = true;
	particle.SetEmittingFlag(0, true);
	for (size_t i = 0; i < 30; ++i)
	{
		trails.SetEmittingFlag(int32_t(i), true);
	}
	trails.Clear();
}

void ParticleOnTrail::Update(float dt)
{
	if (!active_) return;

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

	if (worlds.empty())
	{
		active_ = false;
		trails.Clear();

		for (size_t i = 0; i < 30; ++i)
		{
			trails.SetEmittingFlag(int32_t(i), false);
		}
	}
}

void ParticleOnTrail::Draw()
{
	if (!active_) return;
	//particle.Draw();
	trails.Draw();
}