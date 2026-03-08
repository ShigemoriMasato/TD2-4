#include "IWeapon.h"

void IWeapon::Initialize(int weaponID, Player::Base* player) {
	weaponData_ = weaponManager_->GetWeapon(weaponID);
	player_ = player;
}

void IWeapon::Update(float deltaTime) {
}
