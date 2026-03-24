#pragma once
#include "WeaponData.h"
#include <map>
#include <string>

class WeaponManager;

/// <summary>
/// 武器のパラメータ管理デバッガー
/// </summary>
class WeaponDebugger {
private:
	WeaponManager* weaponManager_;
	int currentWeaponId = 0; // 現在のID

	std::map<WeaponType, std::string> weaponTypeNames_ = {
	    {WeaponType::Pistol,  "Pistol" },
        {WeaponType::Sword,   "Sword"  },
        {WeaponType::ShotGun, "ShotGun"},
        {WeaponType::Spear,   "Spear"  },
	    {WeaponType::Axe,     "Axe"    },
        {WeaponType::Fist,    "Fist"   },
        {WeaponType::Bow,     "Bow"    },
        {WeaponType::Gurepon, "Gurepon"},
	};

public:
	// コンストラクタ
	WeaponDebugger(WeaponManager* weaponManager) : weaponManager_(weaponManager) {}

	// 描画関数
	void Draw();
};
