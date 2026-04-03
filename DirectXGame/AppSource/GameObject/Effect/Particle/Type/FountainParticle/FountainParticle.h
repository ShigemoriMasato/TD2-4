#pragma once
#include <GameObject/Effect/Particle/Particle.h>
#include <GameObject/Effect/Particle/Preset/ParticlePreset.h>

class FountainParticle final
{
public:
	void Initialize(
		SHEngine::DrawDataManager* drawDataManager,
		SHEngine::TextureManager* textureManager,
		SHEngine::ModelManager* modelManager);
	void SetConfig(const FountainConfig& config);
	void Update(float dt, const Matrix4x4& vpMatrix);
	void Draw(CmdObj* cmdObj);

	// 発生位置をセット
	void SetEmitPos(const Vector3& pos);
	// 発生フラグをセット
	void SetEmittingFlag(bool flag);

	void Clear();

	const FountainConfig& GetPreset() const { return config_; }
	Particle& GetParticle() { return particle_; }
	const Particle& GetParticle() const { return particle_; }

private:
	Particle particle_{};
	FountainConfig config_{};
};
