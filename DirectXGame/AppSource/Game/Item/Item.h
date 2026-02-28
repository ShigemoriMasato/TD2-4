#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
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

struct Item {
	// 名前 ← アイテムの識別に使うため重複不可
	std::wstring name;
	// カテゴリ
	Category category;
	// 効果
	uint32_t effect;
	// 2Dタイルの形状データ
	std::vector<std::pair<int, int>> mapData;
	//プレイヤーステータスバフの情報 例：{"HP", 10} -> HPを10上げる
	std::unordered_map<std::string, float> params;	

	//見た目	
	int weaponID = -1;	//武器ID (武器以外は-1)
	int modelID = -1;						//モデルID
	Vector4 color = Vector4(1, 1, 1, 1);	//アイテムの色
};

struct Weapon : public Item {

};
