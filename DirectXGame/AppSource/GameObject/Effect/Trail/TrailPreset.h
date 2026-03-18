#pragma once
#include <GameObject/Effect/Trail/Trail.h>
#include <string>

enum class TrailPresetType
{
	RibbonTrail,
	ShockwaveRing,
};

inline const char* ToString(TrailPresetType t)
{
	switch (t)
	{
	case TrailPresetType::RibbonTrail: return "RibbonTrail";
	case TrailPresetType::ShockwaveRing: return "ShockwaveRing";
	default: return "Unknown";
	}
}

inline bool FromString(const std::string& s, TrailPresetType& out)
{
	if (s == "RibbonTrail") { out = TrailPresetType::RibbonTrail; return true; }
	if (s == "ShockwaveRing") { out = TrailPresetType::ShockwaveRing; return true; }
	return false;
}

struct RibbonTrailPreset
{
	Trail::Config cfg{};
	std::string modelName;   // 表示/運用で紐づけたいモデル識別子
	Vector3 originLocal{ 0.0f, 0.55f, 1.2f };
	Vector3 tipLocal{ 0.0f, 0.55f, -3.2f };
};

struct ShockwaveRingPreset
{
	Trail::Config cfg{};

	// 形状（Trail::Configとは別）
	int segments = 32;           // リング分割数（Push回数）
	float duration = 0.50f;      // 発生時間
	float radiusStart = 0.0f;    // 初期半径
	float radiusEnd = 6.0f;      // 終端半径
	float thickness = 0.5f;      // base-tip 間の太さ（リング帯の幅）

	// 任意：ギザギザ/乱れ（0で無効）
	float noiseAmp = 0.0f;
	float noiseFreq = 1.0f;
};