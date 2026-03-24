#include "Gurepon.h"
#include "GameObject/Attack/GrenadeBullet.h"

void Gurepon::Initialize(int weaponID, Player::Base* player) {
	IRangedWeapon::Initialize(weaponID, player);

	config_.speed *= 0.6f;
}

void Gurepon::Shot(IEnemy* target) {
	isAnimation_ = true;
	std::unique_ptr<GrenadeBullet> bullet = std::make_unique<GrenadeBullet>();
	bullet->Initialize(config_);
	attackManager_->AddObj(std::move(bullet));
}