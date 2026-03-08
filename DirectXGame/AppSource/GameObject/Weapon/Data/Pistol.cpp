#include "Pistol.h"
#include <GameObject/Attack/Bullet.h>

void Pistol::Initialize(int weaponID, Player::Base* player) {
	IWeapon::Initialize(weaponID, player);

	// 武器データベースからパラメータを取得
	rate_ = 2.0f / (weaponData_->attackSpeed);
	speed_ = weaponData_->range;
	range_ = speed_ * lifeTime_;
	bulletNum_ = 1.0f;
	penetration_ = 1.0f + float(weaponData_->penetration);
	spreadAngle_ = weaponData_->spreadAngle;

	config_.spreadAngle = spreadAngle_;
	config_.attackPower = weaponData_->baseDamage;
	config_.range = range_;
	config_.speed = speed_;
	config_.knockbackPower = weaponData_->knockbackPower;
	config_.criticalChance = weaponData_->criticalChance;
	config_.criticalMultiplier = weaponData_->criticalMultiplier;
	config_.lifeSteelChance = weaponData_->lifeStealChance;
	config_.penetration = penetration_;
}

void Pistol::Update(float deltaTime) {
	auto enemies = enemyManager_->GetEnemies();
	rateTimer_ += deltaTime;

	if (rateTimer_ < rate_) {
		return;
	}

	int shotCount = 0;

	// 発射レートに基づいて攻撃を生成
	for (const auto& enemy : enemies) {
		auto drawInfo = enemy->GetDrawInfo();
		Vector3 ePos = drawInfo.position;
		Vector3 pPos = player_->GetTransform().position;

		float distance = (ePos - pPos).Length();

		//一定距離内に敵がいた場合(少し射程に余裕を持たせる)
		if (distance <= (range_ * 0.8f)) {
			
			// 敵の方向に弾を発射
			Vector3 dir = (ePos - pPos).Normalize();
			std::unique_ptr<Bullet> bullet = std::make_unique<Bullet>();
			config_.position = pPos;
			config_.direction = atan2f(dir.z, dir.x);
			bullet->Initialize(config_);
			attackManager_->AddObj(std::move(bullet));

			rateTimer_ = 0.0f;

			if (++shotCount >= bulletNum_) {
				break;
			}
		}
	}

}
