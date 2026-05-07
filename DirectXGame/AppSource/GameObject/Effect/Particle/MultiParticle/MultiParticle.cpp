#include "MultiParticle.h"
#include <stdexcept>
#include <Scene/CommonData.h>
#include <GameObject/Effect/Particle/IParticle.h>
#include <GameObject/Effect/Particle/Preset/ParticlePreset.h>
#include <GameObject/Effect/Particle/Drawer/ParticleDrawer.h>

#include <GameObject/Effect/Particle/Type/GoToTargetParticle/GoToTargetParticle.h>
#include <GameObject/Effect/Particle/Type/B_S_R_T_C_Particle/B_S_R_T_C_Particle.h>

int32_t MultiParticle::Add(const std::string& presetName)
{
	if (!presetData_) return -1;

	// presetName.Jsonのプリセットデータを取得
	const auto& presetVar = presetData_->Get(presetName);

	nextId_++;


	std::unique_ptr<IParticle> particle;

	// パーティクルタイプごとに生成
	if (std::holds_alternative<OnTrailConfig>(presetVar))
	{
		const auto& preset = std::get<OnTrailConfig>(presetVar);
		//particle = std::make_unique<OnTrailParticle>();
		//particle->Initialize(textureManager_, modelManager_);
		//particle->SetConfig(preset);
	}
	else if (std::holds_alternative<GoToTargetConfig>(presetVar))
	{
		const auto& preset = std::get<GoToTargetConfig>(presetVar);
		particle = std::make_unique<GoToTargetParticle>();
		particle->Initialize(textureManager_, modelManager_);
		particle->SetConfig(preset);
	}
	else if (std::holds_alternative<B_S_R_T_C_Config>(presetVar))
	{
		const auto& preset = std::get<B_S_R_T_C_Config>(presetVar);
		particle = std::make_unique<B_S_R_T_C_Particle>();
		particle->Initialize(textureManager_, modelManager_);
		particle->SetConfig(preset);
	}
	else
	{
		throw std::runtime_error("Invalid particle preset type");
	}

	instanceCache_[nextId_] = std::move(particle);

	return nextId_;
}

void MultiParticle::SetCameraPos(const Vector3& cameraPos)
{
	for (auto& [name, particle] : instanceCache_)
	{
		particle->SetCameraPos(cameraPos);
	}
}

void MultiParticle::SetModelWorld(const Matrix4x4& modelWorld)
{
	for (auto& [name, particle] : instanceCache_)
	{
		particle->SetModelWorld(modelWorld);
	}
}

void MultiParticle::SetEmittingFlag(const int32_t id, bool flag)
{
	if (instanceCache_.count(id))
	{
		instanceCache_.at(id)->SetEnabled(flag);
	}
}

void MultiParticle::SetConfig(const int32_t id, const ParticlePresetVariant& presetVar)
{
	if (instanceCache_.count(id))
	{
		instanceCache_.at(id)->SetConfig(presetVar);
	}
}

ParticlePresetVariant MultiParticle::GetConfig(const int32_t id)
{
	if (instanceCache_.count(id))
	{
		return instanceCache_.at(id)->GetUniqueConfig();
	}
	return {};
}

void MultiParticle::Clear()
{
	for (auto& [name, particle] : instanceCache_)
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

	instanceCache_.clear();
}

void MultiParticle::Update(float dt)
{
	for (auto& [name, particle] : instanceCache_)
	{
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

	for (auto& [id, p] : instanceCache_)
	{
		drawer_->Register(p.get());
	}
}

std::vector<Matrix4x4> MultiParticle::GetParticleWorlds(const int32_t id)
{
	if (instanceCache_.count(id))
	{
		return instanceCache_.at(id)->GetParticleWorlds();
	}
	return {};
}

size_t MultiParticle::GetAliveCount(const int32_t id) const
{
	if (instanceCache_.count(id))
	{
		return instanceCache_.at(id)->GetAliveCount();
	}
	return 0;
}