#pragma once
#include "IRangedWeapon.h"

class Gurepon : public IRangedWeapon {
public:
	void Initialize(int weaponID, Player::Base* player) override;

protected:
	void Shot(IEnemy* target) override;
};