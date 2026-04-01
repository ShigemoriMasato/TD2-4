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
void ParticlePresetDataBank::SaveParticleSRT(JsonManager& json, const std::string& keyPrefix, const Particle::ParticleSRT& srt)
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
void ParticlePresetDataBank::Save(const std::string& name, ParticleType type, const Particle::Config& cfg)
{
	json_.Boot("Particle/" + name);

	// type
	{
		std::string typeStr = ToString(type);
		json_.Add("type", typeStr);
	}

	// cfg
	{
		json_.Add("cfg.lifeTime", cfg.lifeTime);
		json_.Add("cfg.speed", cfg.speed);
		json_.Add("cfg.emitNum", cfg.emitNum);
		json_.Add("cfg.emitInterval", cfg.emitInterval);
		json_.Add("cfg.texturePath", cfg.texturePath);
		json_.Add("cfg.modelPath", cfg.modelPath);
	}

	// init
	SaveParticleSRT(json_, "init.scale", cfg.scale);
	SaveParticleSRT(json_, "init.rotate", cfg.rotate);
	SaveParticleSRT(json_, "init.translate", cfg.translate);

	// move
	{
		bool b = cfg.isMoveToTarget;
		auto target = cfg.TargetPos;
		auto moveSpeed = cfg.moveSpeed;

		json_.Add("move.isMoveToTarget", b);
		json_.Add("move.targetPos", target);
		json_.Add("move.moveSpeed", moveSpeed);
	}

	json_.Save();

	// 保存したらキャッシュも更新
	Invalidate(name);
}

// Load
Particle::ParticleSRT ParticlePresetDataBank::LoadParticleSRT(JsonManager& json, const std::string& keyPrefix)
{
	Particle::ParticleSRT outSrt{};

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

	cfg.scale = LoadParticleSRT(json, "init.scale");
	cfg.rotate = LoadParticleSRT(json, "init.rotate");
	cfg.translate = LoadParticleSRT(json, "init.translate");

	try { cfg.isMoveToTarget = json.Get<bool>("move.isMoveToTarget"); }
	catch (...) {}
	try { cfg.TargetPos = json.Get<Vector3>("move.targetPos"); }
	catch (...) {}
	try { cfg.moveSpeed = json.Get<float>("move.moveSpeed"); }
	catch (...) {}

	cfg.lifeTime = std::max(0.001f, cfg.lifeTime);
	cfg.speed = std::max(0.0f, cfg.speed);
	cfg.emitNum = std::max(0, cfg.emitNum);
	cfg.emitInterval = std::max(0.0f, cfg.emitInterval);
	cfg.moveSpeed = std::max(0.0f, cfg.moveSpeed);

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