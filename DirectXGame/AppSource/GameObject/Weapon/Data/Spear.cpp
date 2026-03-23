#include "Spear.h"
#include <GameObject/Attack/Bullet.h>

void Spear::Initialize(int weaponID, Player::Base* player) {
	IMeleeWeapon::Initialize(weaponID, player);
	config_.range /= 3.0f;
	config_.speed = 0.0f;
}

void Spear::Shot(IEnemy* target) {
	if (!target) {
		return;
	}

	Vector3 ePos = target->GetPosition();
	Vector3 pPos = player_->GetTransform().position;
	Vector2 dir = Vector2(ePos.x - pPos.x, ePos.z - pPos.z).Normalize();
	config_.direction = atan2f(dir.y, dir.x);
	config_.position = player_->GetTransform().position;

	float range = config_.range;
	constexpr float bulletRadius = 0.5f;
	int ballCount = static_cast<int>(range / (bulletRadius * 2.0f));
	for (int i = 0; i < ballCount; ++i) {
		std::unique_ptr<Bullet> bullet = std::make_unique<Bullet>();
		config_.position = i * bulletRadius * 2.0f * Vector3(dir.x, 0.0f, dir.y) + pPos;
		bullet->Initialize(config_);
		attackManager_->AddObj(std::move(bullet));
	}
}
