#include "WeaponManager.h"

void WeaponManager::InitializeData() {
	database_[0] = {
	    0,             // ID
	    "Short_Sword", // 名前
	    10.0f,         // 基本ダメージ
	    1.0f,          // 攻撃速度
	    30.0f,         // 値段
	    4,             // サイズ
	    1              // レア度
	};
}

WeaponData* WeaponManager::GetWeapon(int id) {
	if (database_.find(id) != database_.end()) {
		return &database_[id];
	}
	return nullptr;
}
