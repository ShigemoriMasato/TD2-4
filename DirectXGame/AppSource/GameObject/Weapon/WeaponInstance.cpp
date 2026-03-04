#include "WeaponInstance.h"
#include "AttackObject/Projectile.h"
#include <GameObject/Enemy/Enemy.h>
#include <GameObject/Enemy/EnemyManager.h>
#include <Utility/MyMath.h>
#include <SHEngine.h>

void WeaponInstance::Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager) {
	modelManager_ = modelManager;
	drawDataManager_ = drawDataManager;
}

void WeaponInstance::Update(const Vector3& playerPos, float deltaTime) {
	// 攻撃オブジェクトを更新
	for (auto& attackObject : attackObjects_) {
		if (attackObject) {
			attackObject->Update(deltaTime);
		}
	}

	// 無効なオブジェクトを削除
	CleanupInactiveObjects();

	// クールダウンを更新
	currentCooldown_ -= deltaTime;

	if (currentCooldown_ <= 0.0f) {
		ExecuteAttack(playerPos);
		currentCooldown_ = cooldownDuration_;
	}
}

Enemy* WeaponInstance::FindNearestEnemy(const Vector3& playerPos) {
	if (!enemyManager_) {
		return nullptr;
	}

	// 敵のリストを取得
	const auto& enemies = enemyManager_->GetEnemies();
	if (enemies.empty()) {
		return nullptr;
	}

	Enemy* nearestEnemy = nullptr;
	float nearestDistance = weaponData_->range; // 武器の射程距離を初期値に設定

	// プレイヤーに最も近い敵を探索
	for (const auto& enemy : enemies) {
		if (!enemy || !enemy->IsAlive()) {
			continue; // 敵が存在しないか、死んでいる場合はスキップ
		}

		Vector3 toEnemy = enemy->GetPosition() - playerPos;
		float distance = toEnemy.Length();

		// 射程圏内かつ現在の最近距離より近い場合は更新
		if (distance <= nearestDistance) {
			nearestEnemy = enemy.get();
			nearestDistance = distance;
		}
	}

	return nearestEnemy;
}

DamageInfo WeaponInstance::CalculateDamageInfo() const {
	DamageInfo damageInfo{};
	damageInfo.damage = weaponData_->baseDamage;
	damageInfo.criticalChance = weaponData_->criticalChance;
	damageInfo.criticalMult = weaponData_->criticalMultiplier;
	damageInfo.knockbackPower = weaponData_->knockbackPower;
	damageInfo.lifeStealChance = weaponData_->lifeStealChance;
	return damageInfo;
}

void WeaponInstance::ExecuteAttack(const Vector3& playerPos) {
	// 敵マネージャーとマネージャーが未初期化の場合はスキップ
	if (!enemyManager_ || !modelManager_ || !drawDataManager_) {
		return;
	}

	Enemy* target = FindNearestEnemy(playerPos);
	if (!target) {
		return;
	}

	DamageInfo damageInfo = CalculateDamageInfo();
	
	// ターゲットへの方向ベクトルを計算
	Vector3 direction = target->GetPosition() - playerPos;
	direction = MyMath::Normalize(direction);

	// Projectileを生成
	auto projectile = std::make_unique<Projectile>(
		damageInfo,
		playerPos,
		direction,
		20.0f // 飛行速度
	);

	// 初期化
	projectile->Initialize(modelManager_, drawDataManager_);

	// 攻撃オブジェクトを追加
	attackObjects_.push_back(std::move(projectile));
}

void WeaponInstance::AddAttackObject(std::unique_ptr<IAttackObject> attackObject) {
	if (attackObject) {
		attackObjects_.push_back(std::move(attackObject));
	}
}

void WeaponInstance::CleanupInactiveObjects() {
	// 攻撃オブジェクトのリストから無効なオブジェクトを削除
	attackObjects_.erase(
		std::remove_if(
			attackObjects_.begin(),
			attackObjects_.end(),
			[](const std::unique_ptr<IAttackObject>& obj) {
				return !obj || !obj->IsActive();
			}
		),
		attackObjects_.end()
	);
}