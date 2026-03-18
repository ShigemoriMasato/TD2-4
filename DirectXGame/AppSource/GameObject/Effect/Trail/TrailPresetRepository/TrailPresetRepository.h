#pragma once
#include "GameObject/Effect/Trail/TrailPresetType.h"
#include "Tool/Json/JsonManager.h"
#include <unordered_map>
#include <variant>

using TrailPresetVariant = std::variant<Ribbon2PointPreset, ShockwaveRingPreset>;

class TrailPresetRepository final
{
public:
	/// @brief `Assets/Json/<name>.json` をロード（キャッシュあり）
	const TrailPresetVariant& Get(const std::string& name);

	/// @brief キャッシュ破棄（エディタでSave後の再読込など）
	void Invalidate(const std::string& name);

	void Clear();

	/// @brief 便利：型判定だけしたい時
	TrailPresetType GetTypeOf(const std::string& name);

private:
	TrailPresetVariant Load_(const std::string& name);
	Trail::Config LoadConfig_(JsonManager& json);

private:
	JsonManager json_;
	std::unordered_map<std::string, TrailPresetVariant> cache_;
};