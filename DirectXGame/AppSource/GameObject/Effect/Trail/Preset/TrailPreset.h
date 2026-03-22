#pragma once
#include <GameObject/Effect/Trail/Trail.h>
#include <string>

enum class TrailType
{
	RibbonTrail,
	ShockwaveRing,

	None
};

inline const char* ToString(TrailType t)
{
	switch (t)
	{
	case TrailType::RibbonTrail: return "RibbonTrail";
	case TrailType::ShockwaveRing: return "ShockwaveRing";
	default: return "Unknown";
	}
}

inline bool FromString(const std::string& s, TrailType& out)
{
	if (s == "RibbonTrail") { out = TrailType::RibbonTrail; return true; }
	if (s == "ShockwaveRing") { out = TrailType::ShockwaveRing; return true; }
	return false;
}

struct RibbonTrailConfig
{
	Trail::Config cfg{};
	std::string modelName;   // 表示/運用で紐づけたいモデル識別子
	Vector3 originLocal{ 0.0f, 0.55f, 1.2f };
	Vector3 tipLocal{ 0.0f, 0.55f, -3.2f };
};

struct ShockwaveRingConfig
{
	Trail::Config cfg{};

	int segments = 32;           // リング分割数（Push回数）
	float duration = 0.50f;      // 発生時間
	float radiusStart = 0.0f;    // 初期半径
	float radiusEnd = 6.0f;      // 終端半径
	float thickness = 0.5f;      // base-tip 間の太さ（リング帯の幅）

	float noiseAmp = 0.0f;
	float noiseFreq = 1.0f;
};