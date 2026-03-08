#include "IWeapon.h"

void IWeapon::Initialize(int weaponID, Player::Base* player) {
	weaponData_ = weaponManager_->GetWeapon(weaponID);
}

void IWeapon::Update(float deltaTime) {
}
