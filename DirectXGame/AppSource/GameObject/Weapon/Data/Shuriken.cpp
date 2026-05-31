#include "Shuriken.h"
#include "GameObject/Attack/ShurikenBullet.h"
#include <../Engine/Assets/Audio/AudioManager.h>

void Shuriken::Initialize(int weaponID, Player::Base* player) { IRangedWeapon::Initialize(weaponID, player); }

void Shuriken::Update(float deltaTime) { IRangedWeapon::Update(deltaTime); }

void Shuriken::Shot(IEnemy* target) {
	isAnimation_ = true;
	AudioManager::GetInstance()->GetData("Shuriken.mp3")->SetVolume(0.25f);
	AudioManager::GetInstance()->GetData("Shuriken.mp3")->Play();

	std::unique_ptr<ShurikenBullet> bullet = std::make_unique<ShurikenBullet>();

	bullet->SetEnemyManager(enemyManager_);
	bullet->Initialize(config_);

	attackManager_->AddObj(std::move(bullet));
}