#include "IRangedWeapon.h"

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
	rate_ = weaponData_->attackSpeed / ((player->GetParameter("AttackSpeed") + 10.f) / 10.f);
	speed_ = config_.speed;
	range_ = config_.range;
	penetration_ = config_.penetration;
	spreadAngle_ = config_.spreadAngle;
	bulletNum_ = weaponData_->attackCount + player->GetParameter("AttackCount");
}

void IRangedWeapon::Update(float deltaTime) {
	rateTimer_ += deltaTime;

	if (rateTimer_ < rate_) {
		isAnimation_ = false;
		return;
	}

	for (int i = 0; i < bulletNum_; ++i) {
		if (!EnemyCheck()) {
			break;
		}
	}
}

bool IRangedWeapon::EnemyCheck() {
	auto enemies = enemyManager_->GetEnemies();

	// 近くに敵がいるかの判別
	for (const auto& enemy : enemies) {
		Vector3 ePos = enemy->GetPosition();
		Vector3 pPos = player_->GetTransform().position;

		float distance = (ePos - pPos).Length();

		//一定距離内に敵がいた場合(少し射程に余裕を持たせる)
		if (distance <= (range_ * 0.8f)) {

			rateTimer_ = 0.0f;

			if (shotIDRegister_) {
				int id = enemy->GetID(); // 敵のIDを取得
				shotEnemyIDs_.push_back(id); // 射撃した敵のIDを保存
			}

			Vector3 dir = (ePos - pPos).Normalize();
			config_.position = pPos;
			config_.direction = atan2f(dir.z, dir.x);

			Shot(enemy);
			return true;
		}
	}

	return false;
}

void IRangedWeapon::Shot(IEnemy* target) {
	// 敵の方向に弾を生成
	isAnimation_ = true;
	std::unique_ptr<Bullet> bullet = std::make_unique<Bullet>();
	bullet->Initialize(config_);
	attackManager_->AddObj(std::move(bullet));
}
