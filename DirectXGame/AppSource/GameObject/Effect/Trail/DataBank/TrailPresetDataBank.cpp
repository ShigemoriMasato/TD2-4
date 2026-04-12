#include "TrailPresetDataBank.h"
#include <stdexcept>
#include <algorithm>

// static故最強
std::unordered_map<std::string, TrailPresetVariant> TrailPresetDataBank::cache_{};

// cache_から取得。存在しない場合はLoad_してから保存する
const TrailPresetVariant& TrailPresetDataBank::Get(const std::string& name)
{
	auto [it, inserted] = cache_.try_emplace(name, Load(name));
	return it->second;
}

// キャッシュ更新(多分エディタでしか使わない)
void TrailPresetDataBank::Invalidate(const std::string& name)
{
	cache_.erase(name);
}

// 全キャッシュ削除(多分エディタでも使わない)
void TrailPresetDataBank::Clear()
{
	cache_.clear();
}

// Getした場合の型を返す
TrailType TrailPresetDataBank::GetTypeOf(const std::string& name)
{
	const auto& v = Get(name);
	if (std::holds_alternative<RibbonTrailConfig>(v)) return TrailType::RibbonTrail;
	else if (std::holds_alternative<ShockwaveRingConfig>(v)) return TrailType::ShockwaveRing;
	return {};
}

// Save
void TrailPresetDataBank::Save(const std::string& name, const Trail::Config& cfg, RibbonTrailConfig& ribbonPreset)
{
	json_.Boot("Trail/" + name);

	// type
	{
		std::string type = "RibbonTrail";
		json_.Add("type", type);
	}

	// cfg
	{
		json_.Add("cfg.maxSegments", cfg.maxSegments);
		json_.Add("cfg.lifeTime", cfg.lifeTime);
		json_.Add("cfg.minDistance", cfg.minDistance);
		json_.Add("cfg.texturePath", cfg.texturePath);
		json_.Add("cfg.color", cfg.color);
	}

	// type固有
	{
		Vector3 o = ribbonPreset.originLocal;
		Vector3 t = ribbonPreset.tipLocal;
		json_.Add("ribbon.originLocal", o);
		json_.Add("ribbon.tipLocal", t);
	}

	json_.Save();
}
void TrailPresetDataBank::Save(const std::string& name, const Trail::Config& cfg, ShockwaveRingConfig& shockPreset)
{
	json_.Boot("Trail/" + name);

	// type
	{
		std::string type = "ShockwaveRing";
		json_.Add("type", type);
	}

	// cfg
	{
		json_.Add("cfg.maxSegments", cfg.maxSegments);
		json_.Add("cfg.lifeTime", cfg.lifeTime);
		json_.Add("cfg.minDistance", cfg.minDistance);
		json_.Add("cfg.texturePath", cfg.texturePath);
		json_.Add("cfg.color", cfg.color);
	}

	// type固有
	{
		json_.Add("shock.segments", shockPreset.segments);
		json_.Add("shock.duration", shockPreset.duration);
		json_.Add("shock.radiusStart", shockPreset.radiusStart);
		json_.Add("shock.radiusEnd", shockPreset.radiusEnd);
		json_.Add("shock.thickness", shockPreset.thickness);
		json_.Add("shock.noiseAmp", shockPreset.noiseAmp);
		json_.Add("shock.noiseFreq", shockPreset.noiseFreq);
	}

	json_.Save();
}

// Load
Trail::Config TrailPresetDataBank::LoadConfig(JsonManager& json)
{
	Trail::Config cfg{};

	try { cfg.maxSegments = json.Get<int>("cfg.maxSegments"); }
	catch (...) {}
	try { cfg.lifeTime = json.Get<float>("cfg.lifeTime"); }
	catch (...) {}
	try { cfg.minDistance = json.Get<float>("cfg.minDistance"); }
	catch (...) {}
	try { cfg.texturePath = json.Get<std::string>("cfg.texturePath"); }
	catch (...) {}
	try { cfg.color = json.Get<Vector4>("cfg.color"); }
	catch (...) {}

	return cfg;
}
TrailPresetVariant TrailPresetDataBank::Load(const std::string& name)
{
	json_.Boot("Trail/" + name);

	std::string typeStr;
	try { typeStr = json_.Get<std::string>("type"); }
	catch (...) { throw std::runtime_error("TrailPresetDataBank: missing key 'type'"); }

	TrailType type{};
	if (!FromString(typeStr, type))
	{
		throw std::runtime_error("TrailPresetDataBank: unknown type '" + typeStr + "'");
	}

	if (type == TrailType::RibbonTrail)
	{
		RibbonTrailConfig p{};
		p.cfg = LoadConfig(json_);

		try { p.originLocal = json_.Get<Vector3>("ribbon.originLocal"); }
		catch (...) {}
		try { p.tipLocal = json_.Get<Vector3>("ribbon.tipLocal"); }
		catch (...) {}

		return p;
	}
	else if (type == TrailType::ShockwaveRing)
	{
		ShockwaveRingConfig p{};
		p.cfg = LoadConfig(json_);

		try { p.segments = json_.Get<int>("shock.segments"); }
		catch (...) {}
		try { p.duration = json_.Get<float>("shock.duration"); }
		catch (...) {}
		try { p.radiusStart = json_.Get<float>("shock.radiusStart"); }
		catch (...) {}
		try { p.radiusEnd = json_.Get<float>("shock.radiusEnd"); }
		catch (...) {}
		try { p.thickness = json_.Get<float>("shock.thickness"); }
		catch (...) {}
		try { p.noiseAmp = json_.Get<float>("shock.noiseAmp"); }
		catch (...) {}
		try { p.noiseFreq = json_.Get<float>("shock.noiseFreq"); }
		catch (...) {}

		p.segments = std::max(3, p.segments);
		p.duration = std::max(0.01f, p.duration);
		p.thickness = std::max(0.0f, p.thickness);
		p.noiseFreq = std::max(0.0f, p.noiseFreq);

		return p;
	}

	return {};
}
