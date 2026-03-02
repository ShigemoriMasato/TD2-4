#include "WeaponManager.h"
#include "WeaponDatabase.h"

void WeaponManager::InitializeData() {
	// JsonManagerの生成
	jsonManager_ = std::make_unique<JsonManager>();

	// WeaponDatabaseから武器のデータを取得する
	LoadWeaponData();
}

WeaponData* WeaponManager::GetWeapon(int id) {
	if (database_.find(id) != database_.end()) {
		return &database_[id];
	}
	return nullptr;
}

void WeaponManager::SaveWeaponData() {
	// WeaponDatabaseから武器のデータを取得する
	for (const auto& [id, weapon] : database_) {
		jsonManager_->Boot("WeaponData" + std::to_string(id));
		jsonManager_->Add("id", weapon.id);                   // ID
		jsonManager_->Add("baseDamage", weapon.baseDamage);   // 基本ダメージ
		jsonManager_->Add("attackSpeed", weapon.attackSpeed); // 攻撃速度
		jsonManager_->Add("size", weapon.size);               // サイズ
		jsonManager_->Save();
	}
}

void WeaponManager::LoadWeaponData() {
	// WeaponDatabaseから武器のデータを取得する
	for (int i = 0; i < kWeaponDatabaseCount; ++i) {
		jsonManager_->Boot("WeaponData" + std::to_string(kWeaponDatabase[i].id));
		database_[kWeaponDatabase[i].id].id = jsonManager_->Get<int>("id");
		database_[kWeaponDatabase[i].id].baseDamage = jsonManager_->Get<float>("baseDamage");
		database_[kWeaponDatabase[i].id].attackSpeed = jsonManager_->Get<float>("attackSpeed");
		database_[kWeaponDatabase[i].id].size = jsonManager_->Get<int>("size");
	}
}
