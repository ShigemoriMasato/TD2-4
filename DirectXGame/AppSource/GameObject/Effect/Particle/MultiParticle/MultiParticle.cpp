#include "MultiParticle.h"
#include <stdexcept>
#include <GameObject/Effect/Particle/Particle.h>
#include <GameObject/Effect/Particle/Preset/ParticlePreset.h>

void MultiParticle::Initialize(
	SHEngine::DrawDataManager* drawDataManager,
	SHEngine::TextureManager* textureManager,
	SHEngine::ModelManager* modelManager,
	ParticlePresetDataBank* presetData)
{
	drawDataManager_ = drawDataManager;
	textureManager_ = textureManager;
	modelManager_ = modelManager;
	presetData_ = presetData;

	nextId_ = -1;

	fountainCache_.clear();
}


int32_t MultiParticle::Add(const std::string& presetName)
{
	if (!presetData_) return -1;

	nextId_++;

	// presetName.Jsonのプリセットデータを取得
	const auto& presetVar = presetData_->Get(presetName);

	// パーティクルタイプごとに生成
	if (std::holds_alternative<FountainConfig>(presetVar))
	{
		const auto& preset = std::get<FountainConfig>(presetVar);
		auto particle = std::make_unique<FountainParticle>();
		particle->Initialize(drawDataManager_, textureManager_, modelManager_);
		particle->SetConfig(preset);
		fountainCache_[nextId_] = std::move(particle);
	}
	else if (std::holds_alternative<OnTrailConfig>(presetVar))
	{
		const auto& preset = std::get<OnTrailConfig>(presetVar);

	}

	return nextId_;
}

void MultiParticle::SetEmitPos(const int32_t id, const Vector3& pos)
{
	if (fountainCache_.count(id))
	{
		fountainCache_.at(id)->GetParticle().SetEmitPos(pos);
	}
	else
	{
	}
}

void MultiParticle::SetEmittingFlag(const int32_t id, bool flag)
{
	if (fountainCache_.count(id))
	{
		fountainCache_.at(id)->GetParticle().SetEmittingFlag(flag);
	}
	else
	{
		
	}
}

std::vector<Matrix4x4> MultiParticle::GetParticleWorlds(const int32_t id)
{
	if (fountainCache_.count(id))
	{
		return fountainCache_.at(id)->GetParticle().GetParticleWorlds();
	}
	else
	{
		return {};
	}
}

size_t MultiParticle::GetAliveCount(const int32_t id) const
{
	if (fountainCache_.count(id))
	{
		return fountainCache_.at(id)->GetParticle().GetAliveCount();
	}
	else
	{
		return 0;
	}
}

void MultiParticle::Clear()
{
	fountainCache_.clear();
}

void MultiParticle::Update(float dt, const Matrix4x4& vpMatrix)
{
	for (auto& [name, p] : fountainCache_)
	{
		p->Update(dt, vpMatrix);
	}
}

void MultiParticle::Draw(CmdObj* cmdObj)
{
	for (auto& [name, p] : fountainCache_)
	{
		p->Draw(cmdObj);
	}
}