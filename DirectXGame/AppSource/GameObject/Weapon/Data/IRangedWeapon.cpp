#include "IRangedWeapon.h"
#include "GameObject/Attack/PistolBullet.h"
#include <Shop/Piece.h>

void IRangedWeapon::Initialize(int weaponID, Player::Base* player) {
	IWeapon::Initialize(weaponID, player);

	config_.spreadAngle = weaponData_->spreadAngle;
	config_.damage = player->GetParameter("Damage") + (weaponData_->baseDamage + player->GetParameter("RangedDamage")) + weaponData_->rarity * 2.0f;
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

	// 連続攻撃設定
	attackNum_ = bulletNum_;
	if (attackNum_ < 1.0f) {
		attackNum_ = 1.0f;
	}

	// 発射間隔はクールダウンを発射回数で割る
	attackRate_ = rate_ / attackNum_;

	// 初期状態
	rateTimer_ = 0.0f;
	isAttacking_ = false;
	attackCount_ = 0.0f;
}

void IRangedWeapon::Update(float deltaTime) {
	if (!isAttacking_) {
		rateTimer_ += deltaTime;
		if (rateTimer_ > rate_) {
			isAttacking_ = true;
			// 次の分岐で即発射されるよう大きめの値にしておく
			rateTimer_ = 100.0f;
		} else {
			isAnimation_ = false;
			return;
		}
	}

	rateTimer_ += deltaTime;
	if (rateTimer_ > attackRate_) {
		rateTimer_ = 0.0f;

		// ターゲットの有無に関わらず発射
		EnemyCheck();

		// 攻撃回数が最大に達したら攻撃終了
		if (attackCount_ >= attackNum_) {
			isAttacking_ = false;
			attackCount_ = 0.0f;
			rateTimer_ = 0.0f;
		}
	}
}

bool IRangedWeapon::EnemyCheck() {
	auto enemies = enemyManager_->GetEnemies();
	// 一番近くの敵を探す
	float closestDistance = range_ * 2.0f * 10.0f;
	IEnemy* closestEnemy = nullptr;
	Vector3 pPos = player_->GetTransform().position;

	for (const auto& enemy : enemies) {
		Vector3 ePos = enemy->GetPosition();
		float distance = (ePos - pPos).Length();

		if (closestDistance > distance) {
			closestDistance = distance;
			closestEnemy = enemy;
		}
	}

	// 近い敵がいればID登録
	if (closestEnemy && shotIDRegister_) {
		int id = closestEnemy->GetID();
		shotEnemyIDs_.push_back(id);
	}

	// 発射に必要な向き・位置を設定
	if (closestEnemy) {
		Vector3 ePos = closestEnemy->GetPosition();
		Vector2 dir = Vector2(ePos.x - pPos.x, ePos.z - pPos.z).Normalize();
		config_.direction = atan2f(dir.y, dir.x);
	}
	config_.position = pPos;

	// レアリティボーナスを動的に反映
	int rarity = piece_ ? static_cast<int>(piece_->GetRarity()) : weaponData_->rarity;
	config_.damage = player_->GetParameter("Damage") + (weaponData_->baseDamage + player_->GetParameter("RangedDamage")) + rarity * 2.0f;

	// 発射
	Shot(closestEnemy);

	// 発射したらアニメーション開始、タイマーはリセット
	isAnimation_ = true;
	rateTimer_ = 0.0f;

	return true;
}

void IRangedWeapon::Shot(IEnemy* target) {
	// 敵の方向に弾を生成
	isAnimation_ = true;
	std::unique_ptr<PistolBullet> bullet = std::make_unique<PistolBullet>();
	bullet->Initialize(config_);
	attackManager_->AddObj(std::move(bullet));

	// 発射回数を管理
	attackCount_ += 1.0f;
}
