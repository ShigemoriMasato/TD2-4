#pragma once
#include "IMeleeWeapon.h"

class Fist final : public IMeleeWeapon {
public:
	void Initialize(int weaponID, Player::Base* player) override;

private:
	void Shot(IEnemy* target) override;
};