#include "Spear.h"
#include <GameObject/Attack/Bullet.h>

void Spear::Initialize(int weaponID, Player::Base* player) {
	IMeleeWeapon::Initialize(weaponID, player);
	config_.range /= 3.0f;
	config_.speed = 0.0f;
}

void Spear::Attack() {
	for (int i = 0; i < 3; ++i) {
		std::unique_ptr<Bullet> bullet = std::make_unique<Bullet>();
		bullet->Initialize(config_);
		attackManager_->AddObj(std::move(bullet));
	}
}
