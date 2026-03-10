#include "IRangedWeapon.h"
#include <GameObject/Attack/Bullet.h>

void IRangedWeapon::Initialize(int weaponID, Player::Base* player) {
	IWeapon::Initialize(weaponID, player);

	config_.spreadAngle = weaponData_->spreadAngle;
	config_.damage = player->GetParameter("Damage") + (weaponData_->baseDamage + player->GetParameter("RangedDamage"));
	config_.speed = weaponData_->range + player->GetParameter("Range");
	config_.range = config_.speed * lifeTime_;
	config_.knockbackPower = weaponData_->knockbackPower + player->GetParameter("KnockBack");
	config_.criticalChance = weaponData_->criticalChance + player->GetParameter("CritChance");
	config_.criticalMultiplier = weaponData_->criticalMultiplier + player->GetParameter("CritMultiplier");
	config_.lifeSteelChance = weaponData_->lifeStealChance + player->GetParameter("LifeSteel");
	config_.penetration = weaponData_->penetration + player->GetParameter("Penetration");

	// 武器データベースからパラメータを取得
	rate_ = 2.0f / (weaponData_->attackSpeed + player->GetParameter("AttackSpeed"));
	speed_ = config_.speed;
	range_ = config_.range;
	penetration_ = config_.penetration;
	spreadAngle_ = config_.spreadAngle;
	bulletNum_ = weaponData_->attackCount + player->GetParameter("AttackCount");
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
