#pragma once
#include "IRangedWeapon.h"

class ShotGun : public IRangedWeapon {
public:

	void Initialize(int weaponID, Player::Base* player) override;
	void Update(float deltaTime) override;

private:

	const int shots_ = 5; //一回に飛ばすショットガンの弾数

};
