#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <array>
#include <Utility/Vector.h>

enum class Category : int {
	Weapon,
	Armor,
	Item,
};

enum class Effect : uint32_t {
	Fire = 1u << 0,
	Ice = 1u << 1,
};

inline uint32_t operator|(Effect lhs, Effect rhs) {
	return static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs);
}
inline uint32_t operator|(uint32_t lhs, Effect rhs) {
	return lhs | static_cast<uint32_t>(rhs);
}
inline uint32_t operator&(uint32_t lhs, Effect rhs) {
	return lhs & static_cast<uint32_t>(rhs);
}
inline uint32_t operator~(Effect effect) {
	return ~static_cast<uint32_t>(effect);
}

struct ItemRankData
{
	// 値段
	int price = 0;
	// 効果
	uint32_t effect = 0u;
	// バフ
	std::unordered_map<std::string, float> params;
};

struct Item {
	/// ランクに関わらず共通のデータ
	std::wstring name;		// 名前 ← アイテムの識別に使うため重複不可
	Category category;		// カテゴリ
	std::vector<std::pair<int, int>> mapData;	// 2Dタイルの形状データ
	int weaponID = -1;						//武器ID (武器以外は-1)
	int modelID = -1;						//モデルID
	Vector4 color = Vector4(1, 1, 1, 1);	//アイテムの色

	/// ランクごとに変わるデータ  値段・効果・バフ
	std::array<ItemRankData, 4> ranks{};
};