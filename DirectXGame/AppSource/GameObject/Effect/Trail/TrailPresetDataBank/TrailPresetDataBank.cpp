#include "TrailPresetDataBank.h"
#include <stdexcept>
#include <algorithm>

const TrailPresetVariant& TrailPresetDataBank::Get(const std::string& name)
{
	auto [it, inserted] = cache_.try_emplace(name, Load_(name));
	return it->second;
}

void TrailPresetDataBank::Invalidate(const std::string& name)
{
	cache_.erase(name);
}

void TrailPresetDataBank::Clear()
{
	cache_.clear();
}

TrailType TrailPresetDataBank::GetTypeOf(const std::string& name)
{
	const auto& v = Get(name);
	if (std::holds_alternative<RibbonTrailConfig>(v)) return TrailType::RibbonTrail;
	else if (std::holds_alternative<ShockwaveRingConfig>(v)) return TrailType::ShockwaveRing;
	return {};
}

Trail::Config TrailPresetDataBank::LoadConfig_(JsonManager& json)
{
	Trail::Config cfg{};

	try { cfg.maxSegments = json.Get<int>("cfg.maxSegments"); }
	catch (...) {}
	try { cfg.lifeTime = json.Get<float>("cfg.lifeTime"); }
	catch (...) {}
	try { cfg.minDistance = json.Get<float>("cfg.minDistance"); }
	catch (...) {}

	try { cfg.colorNormal = json.Get<Vector4>("cfg.colorNormal"); }
	catch (...) {}
	try { cfg.colorAdd = json.Get<Vector4>("cfg.colorAdd"); }
	catch (...) {}

	try { cfg.drawNormal = json.Get<bool>("cfg.drawNormal"); }
	catch (...) {}
	try { cfg.drawAdd = json.Get<bool>("cfg.drawAdd"); }
	catch (...) {}

	try { cfg.defaultTexturePath = json.Get<std::string>("cfg.defaultTexturePath"); }
	catch (...) {}

	return cfg;
}

TrailPresetVariant TrailPresetDataBank::Load_(const std::string& name)
{
	json_.Boot(name);

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
		p.cfg = LoadConfig_(json_);

		try { p.modelName = json_.Get<std::string>("ribbon.modelName"); }
		catch (...) {}
		try { p.originLocal = json_.Get<Vector3>("ribbon.originLocal"); }
		catch (...) {}
		try { p.tipLocal = json_.Get<Vector3>("ribbon.tipLocal"); }
		catch (...) {}

		return p;
	}
	else if (type == TrailType::ShockwaveRing)
	{
		ShockwaveRingConfig p{};
		p.cfg = LoadConfig_(json_);

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