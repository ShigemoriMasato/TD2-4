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

void WeaponManager::SaveWeaponData() {
	int size = static_cast<int>(database_.size());
	binaryManager_.RegisterOutput(&size);

	for (const auto& [id, weapon] : database_) {
		binaryManager_.RegisterOutput(&weapon.id);          // ID
		binaryManager_.RegisterOutput(&weapon.name);        // 名前
		binaryManager_.RegisterOutput(&weapon.baseDamage);  // 基本ダメージ
		binaryManager_.RegisterOutput(&weapon.attackSpeed); // 攻撃速度
		binaryManager_.RegisterOutput(&weapon.price);       // 値段
		binaryManager_.RegisterOutput(&weapon.size);        // サイズ
		binaryManager_.RegisterOutput(&weapon.tier);        // レア度
	}

	binaryManager_.Write(weaponDataFile_);
}

void WeaponManager::LoadWeaponData() {
	auto data = binaryManager_.Read(weaponDataFile_);
	if (data.empty()) {
		return;
	}

	int size = binaryManager_.Reverse<int>(data);

	for (int i = 0; i < size; ++i) {
		WeaponData weapon;
		weapon.id = binaryManager_.Reverse<int>(data);            // ID
		weapon.name = binaryManager_.Reverse<std::string>(data);  // 名前
		weapon.baseDamage = binaryManager_.Reverse<float>(data);  // 基本ダメージ
		weapon.attackSpeed = binaryManager_.Reverse<float>(data); // 攻撃速度
		weapon.price = binaryManager_.Reverse<float>(data);       // 値段
		weapon.size = binaryManager_.Reverse<int>(data);          // サイズ
		weapon.tier = binaryManager_.Reverse<int>(data);          // レア度

		// nameBufferに名前をコピー
		strncpy_s(weapon.nameBuffer, sizeof(weapon.nameBuffer), weapon.name.c_str(), _TRUNCATE);
		weapon.nameBuffer[sizeof(weapon.nameBuffer) - 1] = '\0';

		database_[weapon.id] = weapon;
	}
}
