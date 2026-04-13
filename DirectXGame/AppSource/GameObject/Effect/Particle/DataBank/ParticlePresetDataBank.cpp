#include "ParticlePresetDataBank.h"
#include <stdexcept>
#include <algorithm>

// static故最強
std::unordered_map<std::string, ParticlePresetVariant> ParticlePresetDataBank::cache_{};

// cache_から取得。存在しない場合はLoad_してから保存する
const ParticlePresetVariant& ParticlePresetDataBank::Get(const std::string& name)
{
	auto [it, inserted] = cache_.try_emplace(name, Load(name));
	return it->second;
}

// キャッシュ更新(多分エディタでしか使わない)
void ParticlePresetDataBank::Invalidate(const std::string& name)
{
	cache_.erase(name);
}

// 全キャッシュ削除(多分エディタでも使わない)
void ParticlePresetDataBank::Clear()
{
	cache_.clear();
}

// Getした場合の型を返す
ParticleType ParticlePresetDataBank::GetTypeOf(const std::string& name)
{
	const auto& v = Get(name);
	if (std::holds_alternative<FountainConfig>(v)) return ParticleType::Fountain;
	else if (std::holds_alternative<OnTrailConfig>(v)) return ParticleType::OnTrail;
	return ParticleType::None;
}

// Save
void ParticlePresetDataBank::SaveParticleSRT(JsonManager& json, const std::string& keyPrefix, const ParticleSRT& srt)
{
	// value
	{
		json.Add(keyPrefix + ".isRandomVal", srt.isRandom_value);
		json.Add(keyPrefix + ".val", srt.initial.value);
		json.Add(keyPrefix + ".randomRangeValMin", srt.randomRange_value_min);
		json.Add(keyPrefix + ".randomRangeValMax", srt.randomRange_value_max);
	}

	// velocity
	{
		json.Add(keyPrefix + ".isRandomVel", srt.isRandom_velocity);
		json.Add(keyPrefix + ".vel", srt.initial.velocity);
		json.Add(keyPrefix + ".randomRangeVelMin", srt.randomRange_velocity_min);
		json.Add(keyPrefix + ".randomRangeVelMax", srt.randomRange_velocity_max);
	}

	// acceleration
	{
		json.Add(keyPrefix + ".isRandomAcc", srt.isRandom_acceleration);
		json.Add(keyPrefix + ".acc", srt.initial.acceleration);
		json.Add(keyPrefix + ".randomRangeAccMin", srt.randomRange_acceleration_min);
		json.Add(keyPrefix + ".randomRangeAccMax", srt.randomRange_acceleration_max);
	}
}
void ParticlePresetDataBank::Save(const std::string& name, FountainConfig& uniqueConfig)
{
	// nameに.jsonがついていたら外す
	std::string baseName = name;
	if (baseName.size() > 5 && baseName.substr(baseName.size() - 5) == ".json")
	{
		baseName = baseName.substr(0, baseName.size() - 5);
	}

	json_.Boot("Trail/" + baseName);

	// type
	{
		std::string type = "Fountain";
		json_.Add("type", type);
	}

	// cfg
	{
		json_.Add("cfg.lifeTime", uniqueConfig.cfg.lifeTime);
		json_.Add("cfg.speed", uniqueConfig.cfg.speed);
		json_.Add("cfg.emitNum", uniqueConfig.cfg.emitNum);
		json_.Add("cfg.emitInterval", uniqueConfig.cfg.emitInterval);
		json_.Add("cfg.texturePath", uniqueConfig.cfg.texturePath);
		json_.Add("cfg.modelPath", uniqueConfig.cfg.modelPath);
	}

	// type固有
	{
		SaveParticleSRT(json_, "init.scale", uniqueConfig.scale);
		SaveParticleSRT(json_, "init.rotate", uniqueConfig.rotate);
		SaveParticleSRT(json_, "init.translate", uniqueConfig.translate);
	}

	json_.Save();
}
void ParticlePresetDataBank::Save(const std::string& name, GoToTargetConfig& uniqueConfig)
{
	// nameに.jsonがついていたら外す
	std::string baseName = name;
	if (baseName.size() > 5 && baseName.substr(baseName.size() - 5) == ".json")
	{
		baseName = baseName.substr(0, baseName.size() - 5);
	}

	json_.Boot("Trail/" + baseName);

	// type
	{
		std::string type = "Fountain";
		json_.Add("type", type);
	}

	// cfg
	{
		json_.Add("cfg.lifeTime", uniqueConfig.cfg.lifeTime);
		json_.Add("cfg.speed", uniqueConfig.cfg.speed);
		json_.Add("cfg.emitNum", uniqueConfig.cfg.emitNum);
		json_.Add("cfg.emitInterval", uniqueConfig.cfg.emitInterval);
		json_.Add("cfg.texturePath", uniqueConfig.cfg.texturePath);
		json_.Add("cfg.modelPath", uniqueConfig.cfg.modelPath);
	}

	// type固有
	{
		json_.Add("move.isMoveToTarget", uniqueConfig.isMoveToTarget);
		json_.Add("move.targetPos", uniqueConfig.TargetPos);
		json_.Add("move.moveSpeed", uniqueConfig.moveSpeed);
	}

	json_.Save();

	// 保存したらキャッシュも更新
	Invalidate(name);
}

void ParticlePresetDataBank::Save(const std::string& name, OnTrailConfig& uniqueConfig)
{
	// nameに.jsonがついていたら外す
	std::string baseName = name;
	if (baseName.size() > 5 && baseName.substr(baseName.size() - 5) == ".json")
	{
		baseName = baseName.substr(0, baseName.size() - 5);
	}
	json_.Boot("Trail/" + baseName);
	// type
	{
		std::string type = "OnTrail";
		json_.Add("type", type);
	}
	// cfg
	{
		json_.Add("cfg.lifeTime", uniqueConfig.cfg.lifeTime);
		json_.Add("cfg.speed", uniqueConfig.cfg.speed);
		json_.Add("cfg.emitNum", uniqueConfig.cfg.emitNum);
		json_.Add("cfg.emitInterval", uniqueConfig.cfg.emitInterval);
		json_.Add("cfg.texturePath", uniqueConfig.cfg.texturePath);
		json_.Add("cfg.modelPath", uniqueConfig.cfg.modelPath);
	}
	json_.Save();
	// 保存したらキャッシュも更新
	Invalidate(name);
}


// Load
ParticleSRT ParticlePresetDataBank::LoadParticleSRT(JsonManager& json, const std::string& keyPrefix)
{
	ParticleSRT outSrt{};

	// value
	try { outSrt.isRandom_value = json.Get<bool>(keyPrefix + ".isRandomVal"); }
	catch (...) {}
	try { outSrt.initial.value = json.Get<Vector3>(keyPrefix + ".val"); }
	catch (...) {}
	try { outSrt.randomRange_value_min = json.Get<Vector3>(keyPrefix + ".randomRangeValMin"); }
	catch (...) {}
	try { outSrt.randomRange_value_max = json.Get<Vector3>(keyPrefix + ".randomRangeValMax"); }
	catch (...) {}

	// velocity
	try { outSrt.isRandom_velocity = json.Get<bool>(keyPrefix + ".isRandomVel"); }
	catch (...) {}
	try { outSrt.initial.velocity = json.Get<Vector3>(keyPrefix + ".vel"); }
	catch (...) {}
	try { outSrt.randomRange_velocity_min = json.Get<Vector3>(keyPrefix + ".randomRangeVelMin"); }
	catch (...) {}
	try { outSrt.randomRange_velocity_max = json.Get<Vector3>(keyPrefix + ".randomRangeVelMax"); }
	catch (...) {}

	// acceleration
	try { outSrt.isRandom_acceleration = json.Get<bool>(keyPrefix + ".isRandomAcc"); }
	catch (...) {}
	try { outSrt.initial.acceleration = json.Get<Vector3>(keyPrefix + ".acc"); }
	catch (...) {}
	try { outSrt.randomRange_acceleration_min = json.Get<Vector3>(keyPrefix + ".randomRangeAccMin"); }
	catch (...) {}
	try { outSrt.randomRange_acceleration_max = json.Get<Vector3>(keyPrefix + ".randomRangeAccMax"); }
	catch (...) {}

	return outSrt;
}
Particle::Config ParticlePresetDataBank::LoadConfig(JsonManager& json)
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
ParticlePresetVariant ParticlePresetDataBank::Load(const std::string& name)
{
	json_.Boot("Particle/" + name);

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
		p.cfg = LoadConfig(json_);

		p.scale = LoadParticleSRT(json_, "init.scale");
		p.rotate = LoadParticleSRT(json_, "init.rotate");
		p.translate = LoadParticleSRT(json_, "init.translate");

		return p;
	}
	else if (type == ParticleType::GoToTarget)
	{
		GoToTargetConfig p{};
		p.cfg = LoadConfig(json_);

		try { p.isMoveToTarget = json_.Get<bool>("move.isMoveToTarget"); }
		catch (...) {}
		try { p.TargetPos = json_.Get<Vector3>("move.targetPos"); }
		catch (...) {}
		try { p.moveSpeed = json_.Get<float>("move.moveSpeed"); }
		catch (...) {}

		return p;
	}
	else if (type == ParticleType::OnTrail)
	{
		OnTrailConfig p{};
		p.cfg = LoadConfig(json_);

		return p;
	}

	throw std::runtime_error("ParticlePresetDataBank: unsupported type '" + typeStr + "'");
}