#include "Shuriken.h"
#include "GameObject/Attack/ShurikenBullet.h"

void Shuriken::Initialize(int weaponID, Player::Base* player) { IRangedWeapon::Initialize(weaponID, player); }

void Shuriken::Update(float deltaTime) { IRangedWeapon::Update(deltaTime); }

void Shuriken::Shot(IEnemy* target) {
	isAnimation_ = true;

	std::unique_ptr<ShurikenBullet> bullet = std::make_unique<ShurikenBullet>();

	bullet->SetEnemyManager(enemyManager_);
	bullet->Initialize(config_);

	attackManager_->AddObj(std::move(bullet));
}