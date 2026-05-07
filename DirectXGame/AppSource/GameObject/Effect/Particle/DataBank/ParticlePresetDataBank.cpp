#include "ParticlePresetDataBank.h"
#include <stdexcept>
#include <algorithm>

namespace
{
	std::string removeJsonExtension(const std::string& name)
	{
		if (name.size() > 5 && name.substr(name.size() - 5) == ".json")
		{
			return name.substr(0, name.size() - 5);
		}
		return name;
	}
}

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
	if (std::holds_alternative<GoToTargetConfig>(v)) return ParticleType::GoToTarget;
	else if (std::holds_alternative<B_S_R_T_C_Config>(v)) return ParticleType::B_S_R_T_C;
	return ParticleType::None;
}

// Save
void ParticlePresetDataBank::SaveParticleSRT(JsonManager& json, const std::string& keyPrefix, const ParticleSRT& srt)
{
	// value
	{
		json.Add(keyPrefix + ".isRandomVal", srt.value.isRandom);
		json.Add(keyPrefix + ".val", srt.value.baseValue);
		json.Add(keyPrefix + ".randomRangeValMin", srt.value.randomRange_min);
		json.Add(keyPrefix + ".randomRangeValMax", srt.value.randomRange_max);
	}

	// velocity
	{
		json.Add(keyPrefix + ".isRandomVel", srt.velocity.isRandom);
		json.Add(keyPrefix + ".vel", srt.velocity.baseValue);
		json.Add(keyPrefix + ".randomRangeVelMin", srt.velocity.randomRange_min);
		json.Add(keyPrefix + ".randomRangeVelMax", srt.velocity.randomRange_max);
	}

	// acceleration
	{
		json.Add(keyPrefix + ".isRandomAcc", srt.acceleration.isRandom);
		json.Add(keyPrefix + ".acc", srt.acceleration.baseValue);
		json.Add(keyPrefix + ".randomRangeAccMin", srt.acceleration.randomRange_min);
		json.Add(keyPrefix + ".randomRangeAccMax", srt.acceleration.randomRange_max);
	}
}
void ParticlePresetDataBank::SaveParticleSRTfloat4(JsonManager& json, const std::string& keyPrefix, const ParticleSRTfloat4& srt)
{
	// value
	{
		json.Add(keyPrefix + ".isRandomVal", srt.value.isRandom);
		json.Add(keyPrefix + ".val", srt.value.baseValue);
		json.Add(keyPrefix + ".randomRangeValMin", srt.value.randomRange_min);
		json.Add(keyPrefix + ".randomRangeValMax", srt.value.randomRange_max);
	}

	// velocity
	{
		json.Add(keyPrefix + ".isRandomVel", srt.velocity.isRandom);
		json.Add(keyPrefix + ".vel", srt.velocity.baseValue);
		json.Add(keyPrefix + ".randomRangeVelMin", srt.velocity.randomRange_min);
		json.Add(keyPrefix + ".randomRangeVelMax", srt.velocity.randomRange_max);
	}

	// acceleration
	{
		json.Add(keyPrefix + ".isRandomAcc", srt.acceleration.isRandom);
		json.Add(keyPrefix + ".acc", srt.acceleration.baseValue);
		json.Add(keyPrefix + ".randomRangeAccMin", srt.acceleration.randomRange_min);
		json.Add(keyPrefix + ".randomRangeAccMax", srt.acceleration.randomRange_max);
	}
}
void ParticlePresetDataBank::SaveConfig(JsonManager& json, const std::string& keyPrefix, const ParticleConfig& cfg)
{
	json_.Add(keyPrefix + ".lifeTime", cfg.lifeTime);
	json_.Add(keyPrefix + ".speed", cfg.speed);
	json_.Add(keyPrefix + ".emitNum", cfg.emitNum);
	json_.Add(keyPrefix + ".emitInterval", cfg.emitInterval);
	json_.Add(keyPrefix + ".isBillboard", cfg.isBillboard_);
	json_.Add(keyPrefix + ".texturePath", cfg.texturePath);
	json_.Add(keyPrefix + ".modelPath", cfg.modelPath);
}
void ParticlePresetDataBank::Save(const std::string& name, GoToTargetConfig& uniqueConfig)
{
	// nameに.jsonがついていたら外す
	json_.Boot("Particle/" + removeJsonExtension(name));

	// type
	{
		std::string type = ToString(ParticleType::GoToTarget);
		json_.Add("type", type);
	}

	// cfg
	{
		SaveConfig(json_, "cfg", uniqueConfig.cfg);
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
void ParticlePresetDataBank::Save(const std::string & name, B_S_R_T_C_Config & uniqueConfig)
{
	// nameに.jsonがついていたら外す
	json_.Boot("Particle/" + removeJsonExtension(name));

	// type
	{
		std::string type = ToString(ParticleType::B_S_R_T_C);
		json_.Add("type", type);
	}

	// cfg
	{
		SaveConfig(json_, "cfg", uniqueConfig.cfg);
	}

	// type固有
	{
		SaveParticleSRT(json_, "init.scale", uniqueConfig.scale);
		SaveParticleSRT(json_, "init.rotate", uniqueConfig.rotate);
		SaveParticleSRT(json_, "init.translate", uniqueConfig.translate);
		SaveParticleSRTfloat4(json_, "init.color", uniqueConfig.color);
	}
	json_.Save();
}

// Load
ParticleSRT ParticlePresetDataBank::LoadParticleSRT(JsonManager& json, const std::string& keyPrefix)
{
	ParticleSRT outSrt{};

	// value
	try { outSrt.value.isRandom = json.Get<bool>(keyPrefix + ".isRandomVal"); }
	catch (...) {}
	try { outSrt.value.baseValue = json.Get<Vector3>(keyPrefix + ".val"); }
	catch (...) {}
	try { outSrt.value.randomRange_min = json.Get<Vector3>(keyPrefix + ".randomRangeValMin"); }
	catch (...) {}
	try { outSrt.value.randomRange_max = json.Get<Vector3>(keyPrefix + ".randomRangeValMax"); }
	catch (...) {}

	// velocity
	try { outSrt.velocity.isRandom = json.Get<bool>(keyPrefix + ".isRandomVel"); }
	catch (...) {}
	try { outSrt.velocity.baseValue = json.Get<Vector3>(keyPrefix + ".vel"); }
	catch (...) {}
	try { outSrt.velocity.randomRange_min = json.Get<Vector3>(keyPrefix + ".randomRangeVelMin"); }
	catch (...) {}
	try { outSrt.velocity.randomRange_max = json.Get<Vector3>(keyPrefix + ".randomRangeVelMax"); }
	catch (...) {}

	// acceleration
	try { outSrt.acceleration.isRandom = json.Get<bool>(keyPrefix + ".isRandomAcc"); }
	catch (...) {}
	try { outSrt.acceleration.baseValue = json.Get<Vector3>(keyPrefix + ".acc"); }
	catch (...) {}
	try { outSrt.acceleration.randomRange_min = json.Get<Vector3>(keyPrefix + ".randomRangeAccMin"); }
	catch (...) {}
	try { outSrt.acceleration.randomRange_max = json.Get<Vector3>(keyPrefix + ".randomRangeAccMax"); }
	catch (...) {}

	return outSrt;
}
ParticleSRTfloat4 ParticlePresetDataBank::LoadParticleSRTfloat4(JsonManager& json, const std::string& keyPrefix)
{
	ParticleSRTfloat4 outSrt{};

	// value
	try { outSrt.value.isRandom = json.Get<bool>(keyPrefix + ".isRandomVal"); }
	catch (...) {}
	try { outSrt.value.baseValue = json.Get<Vector4>(keyPrefix + ".val"); }
	catch (...) {}
	try { outSrt.value.randomRange_min = json.Get<Vector4>(keyPrefix + ".randomRangeValMin"); }
	catch (...) {}
	try { outSrt.value.randomRange_max = json.Get<Vector4>(keyPrefix + ".randomRangeValMax"); }
	catch (...) {}

	// velocity
	try { outSrt.velocity.isRandom = json.Get<bool>(keyPrefix + ".isRandomVel"); }
	catch (...) {}
	try { outSrt.velocity.baseValue = json.Get<Vector4>(keyPrefix + ".vel"); }
	catch (...) {}
	try { outSrt.velocity.randomRange_min = json.Get<Vector4>(keyPrefix + ".randomRangeVelMin"); }
	catch (...) {}
	try { outSrt.velocity.randomRange_max = json.Get<Vector4>(keyPrefix + ".randomRangeVelMax"); }
	catch (...) {}

	// acceleration
	try { outSrt.acceleration.isRandom = json.Get<bool>(keyPrefix + ".isRandomAcc"); }
	catch (...) {}
	try { outSrt.acceleration.baseValue = json.Get<Vector4>(keyPrefix + ".acc"); }
	catch (...) {}
	try { outSrt.acceleration.randomRange_min = json.Get<Vector4>(keyPrefix + ".randomRangeAccMin"); }
	catch (...) {}
	try { outSrt.acceleration.randomRange_max = json.Get<Vector4>(keyPrefix + ".randomRangeAccMax"); }
	catch (...) {}

	return outSrt;
}
ParticleConfig ParticlePresetDataBank::LoadConfig(JsonManager& json)
{
	ParticleConfig cfg{};

	try { cfg.lifeTime = json.Get<float>("cfg.lifeTime"); }
	catch (...) {}
	try { cfg.speed = json.Get<float>("cfg.speed"); }
	catch (...) {}
	try { cfg.emitNum = json.Get<int>("cfg.emitNum"); }
	catch (...) {}
	try { cfg.emitInterval = json.Get<float>("cfg.emitInterval"); }
	catch (...) {}
	try { cfg.isBillboard_ = json.Get<bool>("cfg.isBillboard"); }
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
	// nameに.jsonがついていたら外す
	std::string baseName = name;
	if (baseName.size() > 5 && baseName.substr(baseName.size() - 5) == ".json")
	{
		baseName = baseName.substr(0, baseName.size() - 5);
	}
	json_.Boot("Particle/" + baseName);

	std::string typeStr;
	try { typeStr = json_.Get<std::string>("type"); }
	catch (...) { throw std::runtime_error("ParticlePresetDataBank: missing key 'type'"); }

	ParticleType type{};
	if (!FromString(typeStr, type))
	{
		throw std::runtime_error("ParticlePresetDataBank: unknown type '" + typeStr + "'");
	}

	if (type == ParticleType::GoToTarget)
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
	else if (type == ParticleType::B_S_R_T_C)
	{
		B_S_R_T_C_Config p{};
		p.cfg = LoadConfig(json_);
		try { p.billboard = json_.Get<bool>("init.billboard"); }
		catch (...) {}
		p.scale = LoadParticleSRT(json_, "init.scale");
		p.rotate = LoadParticleSRT(json_, "init.rotate");
		p.translate = LoadParticleSRT(json_, "init.translate");
		p.color = LoadParticleSRTfloat4(json_, "init.color");
		return p;
	}

	throw std::runtime_error("ParticlePresetDataBank: unsupported type '" + typeStr + "'");
}