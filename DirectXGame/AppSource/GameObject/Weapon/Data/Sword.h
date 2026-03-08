#pragma once
#include "../IWeapon.h"

class Sword : public IWeapon {
public:

	void Initialize(int weaponID, Player::Base* player) override;
	void Update(float deltaTime) override;
	

private:



};
