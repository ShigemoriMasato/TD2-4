#include "IRangedWeapon.h"
#include <GameObject/Attack/Bullet.h>

void IRangedWeapon::Initialize(int weaponID, Player::Base* player) {
	IWeapon::Initialize(weaponID, player);
	auto para = player->GetParameter();

	config_.spreadAngle = spreadAngle_;
	config_.attackPower = para.damagePercent * (weaponData_->baseDamage + para.rangedDamage);
	config_.speed = weaponData_->range + para.range;
	config_.range = config_.speed * lifeTime_;
	config_.knockbackPower = weaponData_->knockbackPower;
	config_.criticalChance = weaponData_->criticalChance + para.criticalRatePercent;
	config_.criticalMultiplier = weaponData_->criticalMultiplier;
	config_.lifeSteelChance = weaponData_->lifeStealChance + para.lifeStealPercent;
	config_.penetration = weaponData_->penetration + 1.0f;

	// 武器データベースからパラメータを取得
	rate_ = 2.0f / (weaponData_->attackSpeed + para.attackSpeedPercent);
	speed_ = config_.speed;
	range_ = config_.range;
	penetration_ = config_.penetration;
	spreadAngle_ = weaponData_->spreadAngle;
	bulletNum_ = weaponData_->attackCount + 1.0f;
}

void IRangedWeapon::Update(float deltaTime) {
	rateTimer_ += deltaTime;

	if (rateTimer_ < rate_) {
		return;
	}

	for (int i = 0; i < bulletNum_; ++i) {
		if (!Shot()) {
			break;
		}
	}
}

bool IRangedWeapon::Shot() {
	auto enemies = enemyManager_->GetEnemies();

	// 近くに敵がいるかの判別
	for (const auto& enemy : enemies) {
		auto drawInfo = enemy->GetDrawInfo();
		Vector3 ePos = drawInfo.position;
		Vector3 pPos = player_->GetTransform().position;

		float distance = (ePos - pPos).Length();

		//一定距離内に敵がいた場合(少し射程に余裕を持たせる)
		if (distance <= (range_ * 0.8f)) {

			// 敵の方向に弾を生成
			Vector3 dir = (ePos - pPos).Normalize();
			std::unique_ptr<Bullet> bullet = std::make_unique<Bullet>();
			config_.position = pPos;
			config_.direction = atan2f(dir.z, dir.x);
			bullet->Initialize(config_);
			attackManager_->AddObj(std::move(bullet));

			rateTimer_ = 0.0f;

			return true;
		}
	}

	return false;
}
