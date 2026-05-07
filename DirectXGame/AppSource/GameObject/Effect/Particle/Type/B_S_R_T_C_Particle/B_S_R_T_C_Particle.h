#pragma once
#include <GameObject/Effect/Particle/IParticle.h>
#include <GameObject/Effect/Particle/Preset/ParticlePreset.h>

class B_S_R_T_C_Particle : public IParticle
{
public:
	void SetConfig(const ParticlePresetVariant& config) override;
	void Update(float dt) override;

private:

	struct ParticleInstance
	{
		ParticleSRT scale;
		ParticleSRT rotate;
		ParticleSRT translate;
		float age = 0.0f;
		ParticleSRTfloat4 color;
	};

	std::vector<ParticleInstance> instances_;
};
