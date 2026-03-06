#include "IWeapon.h"

void IWeapon::Initialize(int weaponID, Player::Base* player) {
	weaponData_ = weaponManager_->GetWeapon(weaponID);
	playerPos_ = player->GetPositionPtr();
}

void IWeapon::Update(float deltaTime) {
}
