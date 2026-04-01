#pragma once
#include <Render/RenderObject.h>

#include <GameObject/Effect/Trail/DataBank/TrailPresetDataBank.h>
#include <GameObject/Effect/Trail/MultiTrail/MultiTrail.h>
#include <GameObject/Effect/Particle/DataBank/ParticlePresetDataBank.h>
#include <GameObject/Effect/Particle/MultiParticle/MultiParticle.h>

class SparkEffect
{
public:
	void Initialize(
		SHEngine::DrawDataManager* drawDataManager,
		SHEngine::TextureManager* textureManager,
		SHEngine::ModelManager* modelManager,
		TrailPresetDataBank* trailPresetDataBank,
		ParticlePresetDataBank* particlePresetDataBank);
	void Trigger(const Vector3& position);
	void Update(float dt, const Matrix4x4& vpMatrix);
	void Draw(CmdObj* cmdObj);

private:
	MultiParticle particle;
	static constexpr size_t kMaxParticleTrails_ = 20;
	std::vector<std::unique_ptr<MultiTrail>> particleTrails_;
};

