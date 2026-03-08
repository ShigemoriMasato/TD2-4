#include "WeaponManager.h"

void WeaponManager::InitializeData(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager) {
	// JsonManagerの生成
	jsonManager_ = std::make_unique<JsonManager>();

	// WeaponDatabaseを初期化
	weaponDatabase_ = std::make_unique<WeaponDatabase>();
	weaponDatabase_->Initialize(*jsonManager_);

	// ローカルデータベースにコピー
	LoadWeaponData();

	modelManager_ = modelManager;
	drawDataManager_ = drawDataManager;
}

WeaponData* WeaponManager::GetWeapon(int id) {
	if (database_.find(id) != database_.end()) {
		return &database_[id];
	}
	return nullptr;
}

const WeaponData* WeaponManager::GetWeapon(int id) const {
	auto it = database_.find(id);
	if (it != database_.end()) {
		return &it->second;
	}
	return nullptr;
}

void WeaponManager::SaveWeaponData() {
	for (const auto& [id, weapon] : database_) {
		jsonManager_->Boot("WeaponData" + std::to_string(weapon.id));
		jsonManager_->Add("id", weapon.id);
		jsonManager_->Add("type", static_cast<int>(weapon.type));
		jsonManager_->Add("rarity", weapon.rarity);
		jsonManager_->Add("gold", weapon.gold);
		jsonManager_->Add("baseDamage", weapon.baseDamage);
		jsonManager_->Add("criticalChance", weapon.criticalChance);
		jsonManager_->Add("criticalMultiplier", weapon.criticalMultiplier);
		jsonManager_->Add("lifeStealChance", weapon.lifeStealChance);
		jsonManager_->Add("attackSpeed", weapon.attackSpeed);
		jsonManager_->Add("knockbackPower", weapon.knockbackPower);
		jsonManager_->Add("range", weapon.range);
		jsonManager_->Add("penetration", weapon.penetration);
		jsonManager_->Add("spreadAngle", weapon.spreadAngle);
		jsonManager_->Add("size", weapon.size);
		jsonManager_->Add("attackCount", weapon.attackCount);
		jsonManager_->Save();
	}
}

void WeaponManager::LoadWeaponData() {
	database_.clear();
	const auto& allWeapons = weaponDatabase_->GetAllWeapons();
	for (const auto& weapon : allWeapons) {
		database_[weapon.id] = weapon;
	}
}
