#pragma once
#include "../IWeapon.h"

class Pistol : public IWeapon {
public:

	void Initialize(int weaponID, Player::Base* player) override;
	void Update(float deltaTime) override;

private:

	float size_ = 0.5f;
	float rate_ = 3.0f;

	float rateTimer_ = 0.0f;
};
