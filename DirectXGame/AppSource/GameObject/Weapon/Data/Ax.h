#pragma once
#include "IMeleeWeapon.h"

class Ax : public IMeleeWeapon {
public:
	void Initialize(int weaponID, Player::Base* player) override;
	void Update(float deltaTime) override;

protected:
	void Shot(IEnemy* target) override;
};