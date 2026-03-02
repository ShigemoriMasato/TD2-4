#include "WeaponManager.h"
#include "WeaponDatabase.h"

void WeaponManager::InitializeData() {
	// WeaponDatabaseから武器のデータを取得する
	for (int i = 0; i < kWeaponDatabaseCount; ++i) {
		database_[kWeaponDatabase[i].id] = kWeaponDatabase[i];
	}
}

WeaponData* WeaponManager::GetWeapon(int id) {
	if (database_.find(id) != database_.end()) {
		return &database_[id];
	}
	return nullptr;
}

void WeaponManager::SaveWeaponData() {
	// 保存用の一時的なBinaryManagerを作成
	BinaryManager tempManager;

	int size = static_cast<int>(database_.size());
	tempManager.RegisterOutput(&size);

	for (const auto& [id, weapon] : database_) {
		tempManager.RegisterOutput(&weapon.id);          // ID
		tempManager.RegisterOutput(&weapon.baseDamage);  // 基本ダメージ
		tempManager.RegisterOutput(&weapon.attackSpeed); // 攻撃速度
		tempManager.RegisterOutput(&weapon.size);        // サイズ
	}

	tempManager.Write(weaponDataFile_);
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
		weapon.baseDamage = binaryManager_.Reverse<float>(data);  // 基本ダメージ
		weapon.attackSpeed = binaryManager_.Reverse<float>(data); // 攻撃速度
		weapon.size = binaryManager_.Reverse<int>(data);          // サイズ

		// nameBufferを初期化
		std::memset(weapon.nameBuffer, 0, sizeof(weapon.nameBuffer));

		database_[weapon.id] = weapon;
	}
}
