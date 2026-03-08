#include "Sword.h"

void Sword::Initialize(int weaponID, Player::Base* player) {
	IMeleeWeapon::Initialize(weaponID, player);
}

void Sword::Update(float deltaTime) {
	IMeleeWeapon::Update(deltaTime);
}
