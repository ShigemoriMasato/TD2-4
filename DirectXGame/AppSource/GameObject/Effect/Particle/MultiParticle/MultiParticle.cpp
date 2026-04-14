#include "MultiParticle.h"
#include <stdexcept>
#include <Scene/CommonData.h>
#include <GameObject/Effect/Particle/Particle.h>
#include <GameObject/Effect/Particle/Preset/ParticlePreset.h>
#include <GameObject/Effect/Particle/Drawer/ParticleDrawer.h>

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
	else if (std::holds_alternative<GoToTargetConfig>(presetVar))
	{
		const auto& preset = std::get<GoToTargetConfig>(presetVar);
		(void)preset;
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

void MultiParticle::SetConfig(const int32_t id, const ParticlePresetVariant& presetVar)
{
	if (fountainCache_.count(id))
	{
		if (std::holds_alternative<FountainConfig>(presetVar))
		{
			const auto& preset = std::get<FountainConfig>(presetVar);
			fountainCache_.at(id)->SetConfig(preset);
		}
	}
	//else if (.count(id))
	//{
	//	if (std::holds_alternative<GoToTargetConfig>(presetVar))
	//	{
	//		const auto& preset = std::get<GoToTargetConfig>(presetVar);
	//		(void)preset;
	//	}
	//}
	//else if (.count(id))
	//{
	//	if (std::holds_alternative<OnTrailConfig>(presetVar))
	//	{
	//		const auto& preset = std::get<OnTrailConfig>(presetVar);
	//		(void)preset;
	//	}
	//}
}

ParticlePresetVariant MultiParticle::GetConfig(const int32_t id)
{
	if (fountainCache_.count(id))
	{
		FountainConfig preset = fountainCache_.at(id)->GetPreset();
		return preset;
	}
	//else if (.count(id))
	//{
	//	GoToTargetConfig preset = goToTargetCache_.at(id)->GetPreset();
	//	return preset;
	//}
	//else if (.count(id))
	//{
	//	OnTrailConfig preset = onTrailCache_.at(id)->GetPreset();
	//	return preset;
	//}
	return {};
}

void MultiParticle::Clear()
{
	for (auto& [name, particle] : fountainCache_)
	{
		particle->Clear();
	}
}

void MultiParticle::Initialize(SHEngine::TextureManager* textureManager, SHEngine::ModelManager* modelManager, CommonData* commonData)
{
	textureManager_ = textureManager;
	modelManager_ = modelManager;
	presetData_ = &commonData->particlePresetDataBank;
	drawer_ = commonData->particleDrawer.get();

	nextId_ = -1;

	fountainCache_.clear();
	modelWorld_ = Matrix4x4::Identity();
}

void MultiParticle::Update(float dt)
{
	for (auto& [name, particle] : fountainCache_)
	{
		particle->SetModelWorld(modelWorld_);
		particle->Update(dt);
	}
}

void MultiParticle::Draw()
{
	RegisterToDrawer();
}

void MultiParticle::RegisterToDrawer()
{
	if (!drawer_) return;

	for (auto& [id, p] : fountainCache_)
	{
		drawer_->Register(&p->GetParticle());
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