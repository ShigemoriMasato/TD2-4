#pragma once
#include "IRangedWeapon.h"

class Bow : public IRangedWeapon {
public:
	void Initialize(int weaponID, Player::Base* player) override;

private:
};