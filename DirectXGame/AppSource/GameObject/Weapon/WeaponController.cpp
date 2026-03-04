#include "WeaponController.h"

void WeaponController::EquipWeapon(int weaponID) {
	// WeaponManagerからベースとなるデータを取得する
	const WeaponData* data = weaponManager_->GetWeapon(weaponID);
	if (!data)
		return;

	// データに基づいて武器の実体を生成
	// 例として直接push_back
	//weapons_.push_back(std::make_unique<BaseWeapon>(*data));
}

void WeaponController::AddAttackObject(std::unique_ptr<IAttackObject> attackObj) { activeAttacks_.push_back(std::move(attackObj)); }

void WeaponController::Update(float deltaTime) {
	// 武器の更新
	for (auto& weapon : weapons_) {
		weapon->Update(deltaTime, this);
	}

	// 攻撃オブジェクトの更新
	for (auto it = activeAttacks_.begin(); it != activeAttacks_.end();) {
		(*it)->Update(deltaTime);

		// 非アクティブな攻撃オブジェクトを削除
		if (!(*it)->IsActive()) {
			it = activeAttacks_.erase(it);
		} else {
			++it;
		}
	}

	// 無効な攻撃オブジェクトを削除
	activeAttacks_.erase(std::remove_if(activeAttacks_.begin(), activeAttacks_.end(), [](const std::unique_ptr<IAttackObject>& attack) { return !attack->IsActive(); }), activeAttacks_.end());
}

void WeaponController::Draw(CmdObj* cmdObj) {
	// 武器そのものの描画
	for (auto& weapon : weapons_) {
		weapon->Draw(cmdObj);
	}
	// 攻撃エフェクトや弾の描画
	for (auto& attack : activeAttacks_) {
		attack->Draw(cmdObj);
	}
}
