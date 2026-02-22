#pragma once
#include "WeaponData.h"

/// <summary>
/// 武器のパラメータ管理用クラス
/// </summary>
class WeaponManager {
public:
	std::unordered_map<int, WeaponData> database_;

	// データを追加する関数
	void InitializeData();

	// IDから武器データを取得する
	WeaponData* GetWeapon(int id);
};
