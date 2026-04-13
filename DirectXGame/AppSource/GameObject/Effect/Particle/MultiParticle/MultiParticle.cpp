#include "MultiParticle.h"
#include <stdexcept>
#include <GameObject/Effect/Particle/Particle.h>
#include <GameObject/Effect/Particle/Preset/ParticlePreset.h>
#include <GameObject/Effect/Particle/Drawer/ParticleDrawer.h>

void MultiParticle::Initialize(
	SHEngine::TextureManager* textureManager,
	SHEngine::ModelManager* modelManager,
	ParticlePresetDataBank* presetData)
{
	textureManager_ = textureManager;
	modelManager_ = modelManager;
	presetData_ = presetData;

	nextId_ = -1;

	fountainCache_.clear();
	enabled_ = true;
	modelWorld_ = Matrix4x4::Identity();
}

int32_t MultiParticle::Add(const std::string& presetName)
{
	if (!presetData_) return -1;

	// presetName.Jsonのプリセットデータを取得
	const auto& presetVar = presetData_->Get(presetName);

	nextId_++;
	
	// パーティクルタイプごとに生成
	if (std::holds_alternative<FountainConfig>(presetVar))
	{
		const auto& preset = std::get<FountainConfig>(presetVar);
		auto particle = std::make_unique<FountainParticle>();
		particle->Initialize(textureManager_, modelManager_);
		particle->SetConfig(preset);
		fountainCache_[nextId_] = std::move(particle);
	}
	else if (std::holds_alternative<OnTrailConfig>(presetVar))
	{
		const auto& preset = std::get<OnTrailConfig>(presetVar);
		(void)preset;
	}

	return nextId_;
}

void MultiParticle::SetEmittingFlag(const int32_t id, bool flag)
{
	if (fountainCache_.count(id))
	{
		fountainCache_.at(id)->SetEnabled(flag);
	}
}

void MultiParticle::Clear()
{
	for (auto& [name, particle] : fountainCache_)
	{
		particle->Clear();
	}
}

void MultiParticle::Update(float dt)
{
	if (!enabled_) return;

	for (auto& [name, particle] : fountainCache_)
	{
		particle->SetModelWorld(modelWorld_);
		particle->Update(dt);
	}
}

void MultiParticle::RegisterToDrawer(ParticleDrawer* drawer)
{
	if (!drawer) return;

	for (auto& [id, p] : fountainCache_)
	{
		drawer->Register(&p->GetParticle());
	}
}

std::vector<Matrix4x4> MultiParticle::GetParticleWorlds(const int32_t id)
{
	if (fountainCache_.count(id))
	{
		return fountainCache_.at(id)->GetParticle().GetParticleWorlds();
	}
	return {};
}

size_t MultiParticle::GetAliveCount(const int32_t id) const
{
	if (fountainCache_.count(id))
	{
		return fountainCache_.at(id)->GetParticle().GetAliveCount();
	}
	return 0;
}