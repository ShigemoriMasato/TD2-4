#pragma once
#include <GameObject/Effect/Particle/IParticle.h>
#include <GameObject/Effect/Particle/Preset/ParticlePreset.h>

class B_S_C_Particle : public IParticle
{
public:
	void SetConfig(const ParticlePresetVariant& config) override;
	void Update(float dt) override;

private:

	struct ParticleInstance
	{
		ParticleSRT scale;
		ParticleSRTfloat4 color;
		float age = 0.0f;
	};

	std::vector<ParticleInstance> instances_;
};
