#include "TrailPresetRepository.h"
#include <stdexcept>
#include <algorithm>

const TrailPresetVariant& TrailPresetRepository::Get(const std::string& name)
{
	auto it = cache_.find(name);
	if (it != cache_.end())
	{
		return it->second;
	}

	auto preset = Load_(name);
	auto [insIt, _] = cache_.emplace(name, std::move(preset));
	return insIt->second;
}

void TrailPresetRepository::Invalidate(const std::string& name)
{
	cache_.erase(name);
}

void TrailPresetRepository::Clear()
{
	cache_.clear();
}

TrailPresetType TrailPresetRepository::GetTypeOf(const std::string& name)
{
	const auto& v = Get(name);
	if (std::holds_alternative<Ribbon2PointPreset>(v)) return TrailPresetType::Ribbon2Point;
	return TrailPresetType::ShockwaveRing;
}

Trail::Config TrailPresetRepository::LoadConfig_(JsonManager& json)
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

	// 正常化（Trail側でもやるが念のため）
	cfg.maxSegments = std::max(1, cfg.maxSegments);
	cfg.lifeTime = std::max(0.001f, cfg.lifeTime);
	cfg.minDistance = std::max(0.0f, cfg.minDistance);

	return cfg;
}

TrailPresetVariant TrailPresetRepository::Load_(const std::string& name)
{
	json_.Boot(name);

	std::string typeStr;
	try { typeStr = json_.Get<std::string>("type"); }
	catch (...) { throw std::runtime_error("TrailPresetRepository: missing key 'type'"); }

	TrailPresetType type{};
	if (!FromString(typeStr, type))
	{
		throw std::runtime_error("TrailPresetRepository: unknown type '" + typeStr + "'");
	}

	if (type == TrailPresetType::Ribbon2Point)
	{
		Ribbon2PointPreset p{};
		p.cfg = LoadConfig_(json_);

		try { p.modelName = json_.Get<std::string>("ribbon.modelName"); }
		catch (...) {}
		try { p.originLocal = json_.Get<Vector3>("ribbon.originLocal"); }
		catch (...) {}
		try { p.tipLocal = json_.Get<Vector3>("ribbon.tipLocal"); }
		catch (...) {}

		return p;
	}

	ShockwaveRingPreset p{};
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