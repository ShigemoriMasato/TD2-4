#include "IMeleeWeapon.h"
#include <GameObject/Attack/Swing.h>

void IMeleeWeapon::Initialize(int weaponID, Player::Base* player) {
	IWeapon::Initialize(weaponID, player);
	auto para = player->GetParameter();

	config_.spreadAngle = weaponData_->spreadAngle;
	config_.damage = para.damagePercent + (weaponData_->baseDamage + weaponData_->baseDamage);
	config_.speed = para.attackSpeedPercent + weaponData_->attackSpeed;
	config_.range = weaponData_->range + para.range * lifeTime_;
	config_.knockbackPower = weaponData_->knockbackPower + para.knockback;
	config_.criticalChance = weaponData_->criticalChance + para.criticalRatePercent;
	config_.criticalMultiplier = weaponData_->criticalMultiplier;
	config_.lifeSteelChance = weaponData_->lifeStealChance + para.lifeStealPercent;
	config_.penetration = weaponData_->penetration + para.penetration;

	// 武器データベースからパラメータを取得
	rate_ = 2.0f / (weaponData_->attackSpeed + para.attackSpeedPercent);
	range_ = config_.range;
	attackRate_ = rate_ / config_.speed;
	attackCount_ = 0;
}

void IMeleeWeapon::Update(float deltaTime) {
	if (!isAttacking_) {
		rateTimer_ += deltaTime;
		if (rateTimer_ > rate_) {
			isAttacking_ = true;
			rateTimer_ = 100.0f;
		}
	} else {
		rateTimer_ += deltaTime;

		if (rateTimer_ < attackRate_) {
			return;
		}

		//攻撃の当たり判定を出す
		auto enemies = enemyManager_->GetEnemies();
		//一番近くの敵を探す
		float closestDistance = range_ * 2.0f * 10.0f;
		IEnemy* closestEnemy = nullptr;
		for (const auto& enemy : enemies) {
			auto drawInfo = enemy->GetDrawInfo();
			Vector3 ePos = drawInfo.position;
			Vector3 pPos = player_->GetTransform().position;
			float distance = (ePos - pPos).Length();

			if (closestDistance > distance) {
				closestDistance = distance;
				closestEnemy = enemy;
			}
		}

		if (!closestEnemy) {
			attackCount_++;
			rateTimer_ = 0.0f;
			if (attackCount_ >= attackNum_) {
				isAttacking_ = false;
				attackCount_ = 0;
			}
			return;
		}

		Vector3 ePos = closestEnemy->GetDrawInfo().position;
		Vector3 pPos = player_->GetTransform().position;
		Vector2 dir = Vector2(ePos.x - pPos.x, ePos.z - pPos.z).Normalize();
		config_.direction = atan2f(dir.y, dir.x);
		config_.position = player_->GetTransform().position;

		//攻撃オブジェクトを生成
		std::unique_ptr<Swing> swing = std::make_unique<Swing>();
		swing->Initialize(config_);
		attackManager_->AddObj(std::move(swing));

		//攻撃回数を加算し、攻撃回数が最大値に達したら攻撃終了
		attackCount_++;
		rateTimer_ = 0.0f;
		if (attackCount_ >= attackNum_) {
			isAttacking_ = false;
			attackCount_ = 0;
		}
	}
}
