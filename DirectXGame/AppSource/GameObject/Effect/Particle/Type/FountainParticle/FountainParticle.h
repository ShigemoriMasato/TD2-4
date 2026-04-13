#pragma once
#include <GameObject/Effect/Particle/Particle.h>
#include <GameObject/Effect/Particle/Preset/ParticlePreset.h>

class FountainParticle final
{
public:
	void Initialize(
		SHEngine::TextureManager* textureManager,
		SHEngine::ModelManager* modelManager);
	void SetConfig(const FountainConfig& config);
	void Update(float dt);

	void SetModelWorld(const Matrix4x4& modelWorld) { modelWorld_ = modelWorld; }
	void SetEnabled(bool isActive);

	void Clear() { particle_.Clear(); }

	const FountainConfig& GetPreset() const { return config_; }
	Particle& GetParticle() { return particle_; }
	const Particle& GetParticle() const { return particle_; }

private:
	Particle particle_{};
	FountainConfig config_{};
	Matrix4x4 modelWorld_{ Matrix4x4::Identity() };
	bool isActive_ = true;

	float emitTimer_ = 0.0f;

	struct ParticleInstance
	{
		ParticleSRT scale;
		ParticleSRT rotate;
		ParticleSRT translate;
		float age = 0.0f;
		Vector4 color{ 1,1,1,1 };
	};

	std::vector<ParticleInstance> instances_;
};
