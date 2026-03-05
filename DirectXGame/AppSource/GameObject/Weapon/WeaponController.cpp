#include "WeaponController.h"
#include "../AppSource/GameObject/Weapon/WeaponInstance/BatWeapon.h"

void WeaponController::EquipWeapon(int weaponID) {
	// WeaponManagerからベースとなるデータを取得する
	const WeaponData* data = weaponManager_->GetWeapon(weaponID);

	// データに基づいて武器の実体を生成
	switch (data->id) {
	case 1: // バット
	{
		auto weapon = std::make_unique<BatWeapon>(*data, modelManager_, drawDataManager_);
		weapon->SetEnemyManager(enemyManager_);
		weapons_.push_back(std::move(weapon));
		break;
	}
	default:
	{
		// デフォルト武器（バット）
		auto weapon = std::make_unique<BatWeapon>(*data, modelManager_, drawDataManager_);
		weapon->SetEnemyManager(enemyManager_);
		weapons_.push_back(std::move(weapon));
		break;
	}
	}
}

void WeaponController::AddAttackObject(std::unique_ptr<IAttackObject> attackObj) {
	// EnemyManagerを攻撃オブジェクトに設定
	attackObj->SetEnemyManager(enemyManager_);
	activeAttacks_.push_back(std::move(attackObj));
}

void WeaponController::Update(float deltaTime, Matrix4x4 vpMatrix, Vector3 playerPos) {
	// 武器の更新
	for (auto& weapon : weapons_) {
		weapon->Update(deltaTime, this, vpMatrix, playerPos);
	}

	// 攻撃オブジェクトの更新
	for (auto it = activeAttacks_.begin(); it != activeAttacks_.end();) {
		(*it)->Update(deltaTime, vpMatrix);

		// 非アクティブな攻撃オブジェクトを削除
		if (!(*it)->IsActive()) {
			// 即座には削除せず、遅延削除リストへ追加(3フレーム待つ)
			pendingDeletes_.push_back({std::move(*it), 3});
			// リストからは除外する
			it = activeAttacks_.erase(it);
		} else {
			++it;
		}
	}

	// 遅延削除リストの更新
	for (auto it = pendingDeletes_.begin(); it != pendingDeletes_.end();) {
		it->second--; // 待機フレームを減らす

		// 規定のフレーム待ったら完全に削除
		if (it->second <= 0) {
			it = pendingDeletes_.erase(it);
		} else {
			++it;
		}
	}
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
