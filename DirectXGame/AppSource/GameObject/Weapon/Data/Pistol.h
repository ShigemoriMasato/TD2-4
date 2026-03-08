#pragma once
#include "IRangedWeapon.h"

class Pistol : public IRangedWeapon {
public:

	void Initialize(int weaponID, Player::Base* player) override;
	void Update(float deltaTime) override;

private:

};
