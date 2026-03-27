#pragma once
#include <GameObject/Effect/Particle/Particle.h>
#include <GameObject/Effect/Particle/Preset/ParticlePreset.h>

class FountainParticle final
{
public:
	void Initialize(SHEngine::DrawDataManager* drawDataManager, SHEngine::TextureManager* textureManager, SHEngine::ModelManager* modelManager, const FountainConfig& preset);
	void Update(float dt, const Matrix4x4& vpMatrix);
	void Draw(CmdObj* cmdObj);

	void Trigger(const Vector3& position);
	void Stop();

	void Clear();

	const FountainConfig& GetPreset() const { return preset_; }
	Particle& GetParticle() { return particle_; }
	const Particle& GetParticle() const { return particle_; }

private:
	Particle particle_{};
	FountainConfig preset_{};
};
