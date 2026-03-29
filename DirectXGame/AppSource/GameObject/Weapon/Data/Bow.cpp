#include "Bow.h"
#include "GameObject/Attack/Arrow.h"

void Bow::Initialize(int weaponID, Player::Base* player) {
	IRangedWeapon::Initialize(weaponID, player);
	config_.speed *= 0.8f;
	speed_ = config_.speed;
	penetration_ = config_.penetration;
}

void Bow::Shot(IEnemy* target) {
	// 敵の方向に弾を生成
	isAnimation_ = true;
	std::unique_ptr<Arrow> arrow = std::make_unique<Arrow>();
	arrow->Initialize(config_);
	attackManager_->AddObj(std::move(arrow));
}