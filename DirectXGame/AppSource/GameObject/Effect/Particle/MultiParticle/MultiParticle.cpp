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
	if (std::holds_alternative<PhysicsConfig>(presetVar))
	{
		const auto& preset = std::get<PhysicsConfig>(presetVar);
		auto particle = std::make_unique<PhysicsParticle>();
		particle->Initialize(textureManager_, modelManager_);
		particle->SetConfig(preset);
		physicsCache_[nextId_] = std::move(particle);
	}
	else if (std::holds_alternative<OnTrailConfig>(presetVar))
	{
		const auto& preset = std::get<OnTrailConfig>(presetVar);
		(void)preset;
	}
	else if (std::holds_alternative<GoToTargetConfig>(presetVar))
	{
		const auto& preset = std::get<GoToTargetConfig>(presetVar);
		(void)preset;
	}
	else if (std::holds_alternative<BillboardScaleConfig>(presetVar))
	{
		const auto& preset = std::get<BillboardScaleConfig>(presetVar);
		auto particle = std::make_unique<BillboardScaleParticle>();
		particle->Initialize(textureManager_, modelManager_);
		particle->SetConfig(preset);
		billboardScaleCache_[nextId_] = std::move(particle);
	}
	else if (std::holds_alternative<BillboardScale2Config>(presetVar))
	{
		const auto& preset = std::get<BillboardScale2Config>(presetVar);
		(void)preset;
	}
	else
	{
		throw std::runtime_error("Invalid particle preset type");
	}

	return nextId_;
}

void MultiParticle::SetCameraPos(const Vector3& cameraPos)
{
	for (auto& [name, particle] : billboardScaleCache_)
	{
		particle->SetCameraPos(cameraPos);
	}
	//for (auto& [name, particle] : billboardScale2Cache_)
	//{
	//	particle->GetParticle().SetCameraPos(cameraPos_);
	//}
}

void MultiParticle::SetModelWorld(const Matrix4x4& modelWorld)
{
	for (auto& [name, particle] : physicsCache_)
	{
		particle->SetModelWorld(modelWorld);
	}
	//for (auto& [name, particle] : onTrailCache_)
	//{
	//	particle->SetModelWorld(modelWorld);
	//}
	//for (auto& [name, particle] : goToTargetCache_)
	//{
	//	particle->SetModelWorld(modelWorld);
	//}
	for (auto& [name, particle] : billboardScaleCache_)
	{
		particle->SetModelWorld(modelWorld);
	}
	//for (auto& [name, particle] : billboardScale2Cache_)
	//{
	//	particle->SetModelWorld(modelWorld);
	//}}
}

void MultiParticle::SetEmittingFlag(const int32_t id, bool flag)
{
	if (physicsCache_.count(id))
	{
		physicsCache_.at(id)->SetEnabled(flag);
	}
	//else if (onTrailCache_.count(id))
	//{
	//	onTrailCache_.at(id)->SetEnabled(flag);
	//}
	//else if (goToTargetCache_.count(id))
	//{
	//	goToTargetCache_.at(id)->SetEnabled(flag);
	//}
	else if (billboardScaleCache_.count(id))
	{
		billboardScaleCache_.at(id)->SetEnabled(flag);
	}
	//else if (billboardScale2Cache_.count(id))
	//{
	//	billboardScale2Cache_.at(id)->SetEnabled(flag);
	//}
}

void MultiParticle::SetConfig(const int32_t id, const ParticlePresetVariant& presetVar)
{
	if (physicsCache_.count(id))
	{
		if (std::holds_alternative<PhysicsConfig>(presetVar))
		{
			const auto& preset = std::get<PhysicsConfig>(presetVar);
			physicsCache_.at(id)->SetConfig(preset);
		}
	}
	//else if (goToTargetCache_.count(id))
	//{
	//	if (std::holds_alternative<GoToTargetConfig>(presetVar))
	//	{
	//		const auto& preset = std::get<GoToTargetConfig>(presetVar);
	//		goToTargetCache_.at(id)->SetConfig(preset);
	//	}
	//}
	//else if (onTrailCache_.count(id))
	//{
	//	if (std::holds_alternative<OnTrailConfig>(presetVar))
	//	{
	//		const auto& preset = std::get<OnTrailConfig>(presetVar);
	//		onTrailCache_.at(id)->SetConfig(preset);
	//	}
	//}
	else if (billboardScaleCache_.count(id))
	{
		if (std::holds_alternative<BillboardScaleConfig>(presetVar))
		{
			const auto& preset = std::get<BillboardScaleConfig>(presetVar);
			billboardScaleCache_.at(id)->SetConfig(preset);
		}
	}
	//else if (billboardScale2Cache_.count(id))
	//{
	//	if (std::holds_alternative<BillboardScale2Config>(presetVar))
	//	{
	//		const auto& preset = std::get<BillboardScale2Config>(presetVar);
	//		billboardScale2Cache_.at(id)->SetConfig(preset);
	//	}
	//}
}

ParticlePresetVariant MultiParticle::GetConfig(const int32_t id)
{
	if (physicsCache_.count(id))
	{
		PhysicsConfig preset = physicsCache_.at(id)->GetPreset();
		return preset;
	}
	//else if (onTrailCache_.count(id))
	//{
	//	OnTrailConfig preset = onTrailCache_.at(id)->GetPreset();
	//	return preset;
	//}
	//else if (goToTargetCache_.count(id))
	//{
	//	GoToTargetConfig preset = goToTargetCache_.at(id)->GetPreset();
	//	return preset;
	//}
	else if (billboardScaleCache_.count(id))
	{
		BillboardScaleConfig preset = billboardScaleCache_.at(id)->GetPreset();
		return preset;
	}
	//else if (billboardScale2Cache_.count(id))
	//{
	//	BillboardScale2Config preset = billboardScale2Cache_.at(id)->GetPreset();
	//	return preset;
	//}
	return {};
}

void MultiParticle::Clear()
{
	for (auto& [name, particle] : physicsCache_)
	{
		particle->Clear();
	}
	//for (auto& [name, particle] : onTrailCache_)
	//{
	//	particle->Clear();
	//}
	//for (auto& [name, particle] : goToTargetCache_)
	//{
	//	particle->Clear();
	//}
	for (auto& [name, particle] : billboardScaleCache_)
	{
		particle->Clear();
	}
	//for (auto& [name, particle] : billboardScale2Cache_)
	//{
	//	particle->Clear();
	//}
}

void MultiParticle::Initialize(SHEngine::TextureManager* textureManager, SHEngine::ModelManager* modelManager, CommonData* commonData)
{
	textureManager_ = textureManager;
	modelManager_ = modelManager;
	presetData_ = &commonData->particlePresetDataBank;
	drawer_ = commonData->particleDrawer.get();

	nextId_ = -1;

	physicsCache_.clear();
	//onTrailCache_.clear();
	goToTargetCache_.clear();
	billboardScaleCache_.clear();
	//billboardScale2Cache_.clear();
}

void MultiParticle::Update(float dt)
{
	for (auto& [name, particle] : physicsCache_)
	{
		particle->Update(dt);
	}
	//for (auto& [name, particle] : onTrailCache_)
	//{
	//	particle->Update(dt);
	//}
	//for (auto& [name, particle] : goToTargetCache_)
	//{
	//	particle->Update(dt);
	//}
	for (auto& [name, particle] : billboardScaleCache_)
	{
		particle->Update(dt);
	}
	//for (auto& [name, particle] : billboardScale2Cache_)
	//{
	//	particle->Update(dt);
	//}
}

void MultiParticle::Draw()
{
	RegisterToDrawer();
}

void MultiParticle::RegisterToDrawer()
{
	if (!drawer_) return;

	for (auto& [id, p] : physicsCache_)
	{
		drawer_->Register(&p->GetParticle());
	}
	//for (auto& [id, p] : onTrailCache_)
	//{
	//	drawer_->Register(&p->GetParticle());
	//}
	//for (auto& [id, p] : goToTargetCache_)
	//{
	//	drawer_->Register(&p->GetParticle());
	//}
	for (auto& [id, p] : billboardScaleCache_)
	{
		drawer_->Register(&p->GetParticle());
	}
	//for (auto& [id, p] : billboardScale2Cache_)
	//{
	//	drawer_->Register(&p->GetParticle());
	//}
}

std::vector<Matrix4x4> MultiParticle::GetParticleWorlds(const int32_t id)
{
	if (physicsCache_.count(id))
	{
		return physicsCache_.at(id)->GetParticle().GetParticleWorlds();
	}
	//else if (onTrailCache_.count(id))
	//{
	//	return onTrailCache_.at(id)->GetParticle().GetParticleWorlds();
	//}
	//else if (goToTargetCache_.count(id))
	//{
	//	return goToTargetCache_.at(id)->GetParticle().GetParticleWorlds();
	//}
	else if (billboardScaleCache_.count(id))
	{
		return billboardScaleCache_.at(id)->GetParticle().GetParticleWorlds();
	}
	//else if (billboardScale2Cache_.count(id))
	//{
	//	return billboardScale2Cache_.at(id)->GetParticle().GetParticleWorlds();
	//}
	return {};
}

size_t MultiParticle::GetAliveCount(const int32_t id) const
{
	if (physicsCache_.count(id))
	{
		return physicsCache_.at(id)->GetParticle().GetAliveCount();
	}
	//else if (onTrailCache_.count(id))
	//{
	//	return onTrailCache_.at(id)->GetParticle().GetAliveCount();
	//}
	//else if (goToTargetCache_.count(id))
	//{
	//	return goToTargetCache_.at(id)->GetParticle().GetAliveCount();
	//}
	else if (billboardScaleCache_.count(id))
	{
		return billboardScaleCache_.at(id)->GetParticle().GetAliveCount();
	}
	//else if (billboardScale2Cache_.count(id))
	//{
	//	return billboardScale2Cache_.at(id)->GetParticle().GetAliveCount();
	//}

	return 0;
}