#include "Spear.h"
#include <GameObject/Attack/Bullet.h>

void Spear::Initialize(int weaponID, Player::Base* player) {
	IMeleeWeapon::Initialize(weaponID, player);
	config_.range /= 3.0f;
	config_.speed = 0.0f;
}

void Spear::Attack() {
	Vector3 basePos = config_.position;
	Vector3 dir = { cosf(config_.direction), 0.0f, sinf(config_.direction) };
	float range = config_.range;
	constexpr float bulletRadius = 0.5f;
	int ballCount = static_cast<int>(range / (bulletRadius * 2.0f));
	for (int i = 0; i < ballCount; ++i) {
		std::unique_ptr<Bullet> bullet = std::make_unique<Bullet>();
		config_.position = i * bulletRadius * 2.0f * dir + basePos;
		bullet->Initialize(config_);
		attackManager_->AddObj(std::move(bullet));
	}
}
