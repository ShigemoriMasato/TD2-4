#pragma once
#include <string>
#include <vector>

enum class Category : int {
	Weapon,
	Armor,
	Item,
};

enum class Effect {
	Fire,
	Ice,

};

uint32_t operator|(Effect lhs, Effect rhs) {
	return static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs);
}
uint32_t operator|(uint32_t lhs, Effect rhs) {
	return lhs | static_cast<uint32_t>(rhs);
}
uint32_t operator&(uint32_t lhs, Effect rhs) {
	return lhs & static_cast<uint32_t>(rhs);
}
uint32_t operator~(Effect effect) {
	return ~static_cast<uint32_t>(effect);
}

struct ItemParam {
	std::string name;		//バフの名称
	float value;			//値
};

struct Item {
	std::wstring name;
	Category category;
	uint32_t effect;
	std::vector<std::pair<int, int>> mapData;

	std::vector<ItemParam> buffs;		//プレイヤーに干渉するバフの情報
	int weaponID = -1;	//武器ID (武器以外は-1)

	//見た目
	int modelID = -1;						//モデルID
	Vector4 color = Vector4(1, 1, 1, 1);	//アイテムの色
};

struct Weapon : public Item {

};
