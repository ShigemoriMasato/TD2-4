#pragma once
#include <GameObject/Effect/Particle/IParticle.h>
#include <GameObject/Effect/Particle/Preset/ParticlePreset.h>

class B_S_Particle : public IParticle
{
public:
	void SetConfig(const ParticlePresetVariant& config) override;
	void Update(float dt) override;

private:

	struct ParticleInstance
	{
		ParticleSRT scale;
		float age = 0.0f;
		Vector4 color{ 1,1,1,1 };
	};

	std::vector<ParticleInstance> instances_;
};
