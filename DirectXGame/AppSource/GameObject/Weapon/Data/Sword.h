#pragma once
#include "IMeleeWeapon.h"

class Sword : public IMeleeWeapon {
public:

	void Initialize(int weaponID, Player::Base* player) override;
	void Update(float deltaTime) override;
	

private:



};
