#pragma once
#include "WeaponData.h"

class WeaponManager {
public:
	std::unordered_map<int, WeaponData> database;

	// データを追加する関数
	void InitializeData();

	// IDから武器データを取得するGetter
	WeaponData* GetWeapon(int id);
};
