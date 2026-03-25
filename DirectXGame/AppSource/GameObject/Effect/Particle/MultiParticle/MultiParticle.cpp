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

	fountainCache_.clear();
	enabled_ = true;
}

void MultiParticle::Add(const std::string& presetName)
{
	if (!presetData_) return;

	// presetName.Jsonのプリセットデータを取得
	const auto& presetVar = presetData_->Get(presetName);

	// パーティクルタイプごとに生成
	if (std::holds_alternative<FountainConfig>(presetVar))
	{
		const auto& preset = std::get<FountainConfig>(presetVar);
		auto particle = std::make_unique<FountainParticle>();
		particle->Initialize(drawDataManager_, textureManager_, nullptr, preset);
		fountainCache_[presetName] = std::move(particle);
		return;
	}
	else if (std::holds_alternative<OnTrailConfig>(presetVar))
	{
		return;
	}
}

void MultiParticle::Trigger(const std::string& presetName, const Vector3& position)
{
	fountainCache_.at(presetName)->Trigger(position);
}

void MultiParticle::Stop(const std::string& presetName)
{
	fountainCache_.at(presetName)->Stop();
}

void MultiParticle::Clear()
{
	fountainCache_.clear();
}

void MultiParticle::Update(float dt, const Matrix4x4& vpMatrix)
{
	if (!enabled_) return;

	for (auto& [name, p] : fountainCache_)
	{
		p->Update(dt, vpMatrix);
	}
}

void MultiParticle::Draw(CmdObj* cmdObj)
{
	if (!enabled_) return;

	for (auto& [name, p] : fountainCache_)
	{
		p->Draw(cmdObj);
	}
}