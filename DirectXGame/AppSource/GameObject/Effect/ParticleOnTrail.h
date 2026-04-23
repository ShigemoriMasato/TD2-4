#pragma once
#include <GameObject/Effect/Trail/MultiTrail/MultiTrail.h>
#include <GameObject/Effect/Particle/MultiParticle/MultiParticle.h>

class ParticleOnTrail
{
	void Initialize(
		SHEngine::TextureManager* textureManager,
		SHEngine::ModelManager* modelManager,
		CommonData* commonData,
		std::string particlePresetName,
		std::string trailPresetName
	);
	void Update(float dt);
	void Draw();
	void SetModelWorld(const Matrix4x4& modelWorld) { modelWorld_ = modelWorld; }
	void Trigger();


private:
	Matrix4x4 modelWorld_{ Matrix4x4::Identity() };
	MultiParticle particle;
	MultiTrail trails;

	bool active_ = false;
};

