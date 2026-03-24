#include "Ax.h"
#include "GameObject/Attack/AxeSwing.h"

void Ax::Initialize(int weaponID, Player::Base* player) { IMeleeWeapon::Initialize(weaponID, player); }

void Ax::Update(float deltaTime) { IMeleeWeapon::Update(deltaTime); }

void Ax::Shot(IEnemy* target) {
	if (target) {
		Vector3 ePos = target->GetPosition();
		Vector3 pPos = player_->GetTransform().position;
		Vector2 dir = Vector2(ePos.x - pPos.x, ePos.z - pPos.z).Normalize();
		config_.direction = atan2f(dir.y, dir.x);
	}
	config_.position = player_->GetTransform().position;

	std::unique_ptr<AxeSwing> swing = std::make_unique<AxeSwing>();
	swing->Initialize(config_);
	attackManager_->AddObj(std::move(swing));

	attackCount_++;
}