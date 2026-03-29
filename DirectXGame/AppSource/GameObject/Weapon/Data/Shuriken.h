#pragma once
#include "IRangedWeapon.h"

class Shuriken : public IRangedWeapon {
public:
	void Initialize(int weaponID, Player::Base* player) override;
	void Update(float deltaTime) override;

protected:
	void Shot(IEnemy* target) override;
};