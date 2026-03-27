#include "ParticlePresetDataBank.h"
#include <stdexcept>
#include <algorithm>

std::unordered_map<std::string, ParticlePresetVariant> ParticlePresetDataBank::cache_{};

const ParticlePresetVariant& ParticlePresetDataBank::Get(const std::string& name)
{
	auto [it, inserted] = cache_.try_emplace(name, Load_(name));
	return it->second;
}

void ParticlePresetDataBank::Invalidate(const std::string& name)
{
	cache_.erase(name);
}

void ParticlePresetDataBank::Clear()
{
	cache_.clear();
}

ParticleType ParticlePresetDataBank::GetTypeOf(const std::string& name)
{
	const auto& v = Get(name);
	if (std::holds_alternative<FountainConfig>(v)) return ParticleType::Fountain;
	else if (std::holds_alternative<OnTrailConfig>(v)) return ParticleType::OnTrail;
	return ParticleType::None;
}

Particle::Config ParticlePresetDataBank::LoadConfig_(JsonManager& json)
{
	Particle::Config cfg{};

	try { cfg.lifeTime = json.Get<float>("cfg.lifeTime"); }
	catch (...) {}
	try { cfg.speed = json.Get<float>("cfg.speed"); }
	catch (...) {}
	try { cfg.emitNum = json.Get<int>("cfg.emitNum"); }
	catch (...) {}
	try { cfg.emitInterval = json.Get<float>("cfg.emitInterval"); }
	catch (...) {}

	try { cfg.texturePath = json.Get<std::string>("cfg.texturePath"); }
	catch (...) {}
	try { cfg.modelPath = json.Get<std::string>("cfg.modelPath"); }
	catch (...) {}

	cfg.lifeTime = std::max(0.001f, cfg.lifeTime);
	cfg.speed = std::max(0.0f, cfg.speed);
	cfg.emitNum = std::max(0, cfg.emitNum);
	cfg.emitInterval = std::max(0.0f, cfg.emitInterval);

	return cfg;
}

ParticlePresetVariant ParticlePresetDataBank::Load_(const std::string& name)
{
	json_.Boot(name);

	std::string typeStr;
	try { typeStr = json_.Get<std::string>("type"); }
	catch (...) { throw std::runtime_error("ParticlePresetDataBank: missing key 'type'"); }

	ParticleType type{};
	if (!FromString(typeStr, type))
	{
		throw std::runtime_error("ParticlePresetDataBank: unknown type '" + typeStr + "'");
	}

	if (type == ParticleType::Fountain)
	{
		FountainConfig p{};
		p.cfg = LoadConfig_(json_);
		return p;
	}
	else if (type == ParticleType::OnTrail)
	{
		OnTrailConfig p{};
		p.cfg = LoadConfig_(json_);
		return p;
	}

	throw std::runtime_error("ParticlePresetDataBank: unsupported type '" + typeStr + "'");
}