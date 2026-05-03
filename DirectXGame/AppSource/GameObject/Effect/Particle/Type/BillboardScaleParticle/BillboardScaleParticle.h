#pragma once
#include <GameObject/Effect/Particle/Particle.h>
#include <GameObject/Effect/Particle/Preset/ParticlePreset.h>

class BillboardScaleParticle final
{
public:
	void Initialize(
		SHEngine::TextureManager* textureManager,
		SHEngine::ModelManager* modelManager);
	void SetConfig(const BillboardScaleConfig& config);
	void Update(float dt);

	void SetModelWorld(const Matrix4x4& modelWorld) { modelWorld_ = modelWorld; }
	void SetCameraPos(const Vector3& cameraPos) { cameraPos_ = cameraPos; }
	void SetEnabled(bool isActive);

	void Clear() { particle_.Clear(); }

	const BillboardScaleConfig& GetPreset() const { return config_; }
	Particle& GetParticle() { return particle_; }
	const Particle& GetParticle() const { return particle_; }

private:
	Particle particle_{};
	BillboardScaleConfig config_{};
	Matrix4x4 modelWorld_{ Matrix4x4::Identity() };
	bool isActive_ = true;
	float emitTimer_ = 0.0f;
	Vector3 cameraPos_{};

	struct ParticleInstance
	{
		ParticleSRT scale;
		float age = 0.0f;
		Vector4 color{ 1,1,1,1 };
	};

	std::vector<ParticleInstance> instances_;
};
