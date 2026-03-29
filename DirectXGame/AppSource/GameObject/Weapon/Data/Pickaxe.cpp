#include "Pickaxe.h"
#include "GameObject/Attack/AxeSwing.h"
#include "GameObject/Attack/Bullet.h"

void Pickaxe::Initialize(int weaponID, Player::Base* player) {
	IMeleeWeapon::Initialize(weaponID, player);

	// ベースの射程を元に、それぞれの射程を設定
	thrustRange_ = config_.range * 1.5f;
	sweepRange_ = 1.0f;

	isThrust_ = true;
	config_.range = thrustRange_;
	attackNum_ = static_cast<float>(weaponData_->attackCount);
}

void Pickaxe::Update(float deltaTime) { IMeleeWeapon::Update(deltaTime); }

void Pickaxe::Shot(IEnemy* target) {
	if (target) {
		Vector3 ePos = target->GetPosition();
		pPos_ = player_->GetTransform().position;
		dir_ = Vector2(ePos.x - pPos_.x, ePos.z - pPos_.z).Normalize();
		config_.direction = atan2f(dir_.y, dir_.x);
	}
	config_.position = player_->GetTransform().position;

	if (isThrust_) {
		// 突き
		constexpr float bulletRadius = 0.5f;
		int ballCount = static_cast<int>(config_.range / (bulletRadius * 2.0f));
		for (int i = 0; i < ballCount; ++i) {
			std::unique_ptr<Bullet> bullet = std::make_unique<Bullet>();
			config_.position = i * bulletRadius * 2.0f * Vector3(dir_.x, 0.0f, dir_.y) + pPos_;
			bullet->Initialize(config_);
			attackManager_->AddObj(std::move(bullet));
		}
	} else {
		// 薙ぎ払い
		std::unique_ptr<AxeSwing> sweep = std::make_unique<AxeSwing>();
		sweep->Initialize(config_);
		attackManager_->AddObj(std::move(sweep));
	}

	// 次の攻撃に向けてフラグを反転
	isThrust_ = !isThrust_;
	config_.range = isThrust_ ? thrustRange_ : sweepRange_;

	attackCount_++;
}