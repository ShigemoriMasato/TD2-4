#pragma once
#include <map>
#include <string>
#include "WeaponData.h"

class WeaponManager;

/// <summary>
/// 武器のパラメータ管理デバッガー
/// </summary>
class WeaponDebugger {
private:
	WeaponManager* weaponManager_;
	int currentWeaponId = 0; // 現在のID

	std::map<WeaponType, std::string> weaponTypeNames_ = {
		{ WeaponType::Pistol, "Pistol" },
		{ WeaponType::Sowrd, "Sword" },
	};

public:
	// コンストラクタ
	WeaponDebugger(WeaponManager* weaponManager) : weaponManager_(weaponManager) {}

	// 描画関数
	void Draw();
};
