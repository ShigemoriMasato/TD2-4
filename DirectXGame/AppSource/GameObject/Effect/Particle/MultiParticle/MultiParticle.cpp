#include "MultiParticle.h"
#include <stdexcept>
#include <GameObject/Effect/Particle/Particle.h>
#include <GameObject/Effect/Particle/Preset/ParticlePreset.h>

void MultiParticle::Initialize(
	SHEngine::DrawDataManager* drawDataManager,
	SHEngine::TextureManager* textureManager,
	ParticlePresetDataBank* presetData)
{
	drawDataManager_ = drawDataManager;
	textureManager_ = textureManager;
	presetData_ = presetData;

	cache_.clear();
	enabled_ = true;
}

void MultiParticle::Add(const std::string& presetName)
{

}

void MultiParticle::Trigger(const std::string& presetName, const Vector3& position)
{
	cache_.at(presetName)->Trigger(position);
}

void MultiParticle::Stop(const std::string& presetName)
{
	cache_.at(presetName)->Stop();
}

void MultiParticle::Clear()
{
	cache_.clear();
}

void MultiParticle::Update(float dt)
{
	if (!enabled_) return;

	for (auto& [name, p] : cache_)
	{
		p->Update(dt);
	}
}

void MultiParticle::Draw(CmdObj* cmdObj, const Matrix4x4& vpMatrix)
{
	if (!enabled_) return;

	for (auto& [name, p] : cache_)
	{
		p->Draw(cmdObj, vpMatrix);
	}
}