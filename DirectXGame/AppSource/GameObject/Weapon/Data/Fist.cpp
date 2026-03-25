#include "Fist.h"
#include <GameObject/Attack/Bullet.h>

void Fist::Initialize(int weaponID, Player::Base* player) {
	IMeleeWeapon::Initialize(weaponID, player);
	config_.range /= 5.0f;
	config_.speed = 15.0f;
}

void Fist::Shot(IEnemy* target) {
	if (!target) {
		return;
	}

	Vector3 ePos = target->GetPosition();
	Vector3 pPos = player_->GetTransform().position;
	Vector2 dir = Vector2(ePos.x - pPos.x, ePos.z - pPos.z).Normalize();
	config_.direction = atan2f(dir.y, dir.x);

	std::unique_ptr<Bullet> bullet = std::make_unique<Bullet>();
	constexpr float punchOffset = 0.5f;
	config_.position = punchOffset * Vector3(dir.x, 0.0f, dir.y) + pPos;
	bullet->Initialize(config_);
	attackManager_->AddObj(std::move(bullet));
}