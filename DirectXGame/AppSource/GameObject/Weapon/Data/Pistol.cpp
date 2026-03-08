#include "Pistol.h"

void Pistol::Initialize(int weaponID, Player::Base* player) {
	IRangedWeapon::Initialize(weaponID, player);
}

void Pistol::Update(float deltaTime) {
	IRangedWeapon::Update(deltaTime);
}
