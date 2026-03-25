#include "Bow.h"

void Bow::Initialize(int weaponID, Player::Base* player) {
	IRangedWeapon::Initialize(weaponID, player);
	config_.speed *= 0.8f;
	speed_ = config_.speed;
	penetration_ = config_.penetration;
}