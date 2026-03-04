#include "WeaponController.h"
#include "AttackObject/IAttackObject.h"
#include "AttackObject/Projectile.h"
#include <GameObject/Enemy/EnemyManager.h>
#include <SHEngine.h>

void WeaponController::Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager) {
	modelManager_ = modelManager;
	drawDataManager_ = drawDataManager;
}

void WeaponController::AddWeapon(const WeaponData* data) {
	auto weapon = std::make_unique<WeaponInstance>(data);
	weapon->SetEnemyManager(enemyManager_);
	weapon->Initialize(modelManager_, drawDataManager_);
	activeWeapons_.emplace_back(std::move(weapon));
}

void WeaponController::Update(const Vector3& playerPos, float deltaTime) {
	for (auto& weapon : activeWeapons_) {
		weapon->Update(playerPos, deltaTime);
	}

	// 敵との当たり判定をチェック
	CheckCollisionsWithEnemies();
}

void WeaponController::SetEnemyManager(EnemyManager* enemyManager) {
	enemyManager_ = enemyManager;
	for (auto& weapon : activeWeapons_) {
		weapon->SetEnemyManager(enemyManager);
	}
}

void WeaponController::CheckCollisionsWithEnemies() {
	if (!enemyManager_) {
		return;
	}

	const auto& enemies = enemyManager_->GetEnemies();

	// すべての武器の攻撃オブジェクトについて当たり判定をチェック
	for (auto& weapon : activeWeapons_) {
		const auto& attackObjects = weapon->GetAttackObjects();
		for (const auto& attackObj : attackObjects) {
			if (!attackObj || !attackObj->IsActive()) {
				continue;
			}

			// Projectile型の当たり判定処理
			if (auto projectile = dynamic_cast<Projectile*>(attackObj.get())) {
				projectile->CheckCollisionWithEnemies(enemies);
			}
		}
	}
}

void WeaponController::DrawAllAttackObjects() {
	// すべての武器の攻撃オブジェクトを描画
	for (auto& weapon : activeWeapons_) {
		const auto& attackObjects = weapon->GetAttackObjects();
		for (const auto& obj : attackObjects) {
			if (obj && obj->IsActive()) {
				obj->Draw();
			}
		}
	}
}