#pragma once
#include <string>
#include <variant>
#include <Utility/Vector.h>

enum class TrailType
{
	// 点を2点つなげてリボンのように描画するタイプ
	RibbonTrail,
	// 点をめっちゃ出して描画するタイプ。
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

struct TrailConfig
{
	// 履歴
	int maxSegments = 32;      // セグメント数。頂点数は maxSegments*2
	float lifeTime = 0.20f;    // 寿命
	float minDistance = 0.1f; // minDistance以上動いてないときは追加しない

	// 見た目
	Vector4 color = { 0.6f, 0.9f, 1.0f, 0.75f };

	// テクスチャ
	std::string texturePath = "Assets/.EngineResource/Texture/white1x1.png";
};

struct RibbonTrailConfig
{
	TrailConfig cfg{};
	std::string modelName;   // 表示/運用で紐づけたいモデル識別子
	Vector3 originLocal{ 0.0f, 0.55f, 1.2f };
	Vector3 tipLocal{ 0.0f, 0.55f, -3.2f };
};

struct ShockwaveRingConfig
{
	TrailConfig cfg{};

	int segments = 32;           // リング分割数（Push回数）
	float duration = 0.50f;      // 発生時間
	float radiusStart = 0.0f;    // 初期半径
	float radiusEnd = 6.0f;      // 終端半径
	float thickness = 0.5f;      // base-tip 間の太さ（リング帯の幅）

	float noiseAmp = 0.0f;
	float noiseFreq = 1.0f;
};


using TrailPresetVariant = std::variant<
	RibbonTrailConfig, 
	ShockwaveRingConfig
>;