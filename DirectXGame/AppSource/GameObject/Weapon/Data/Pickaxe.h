#pragma once
#include "IMeleeWeapon.h"

class Pickaxe : public IMeleeWeapon {
public:
	void Initialize(int weaponID, Player::Base* player) override;
	void Update(float deltaTime) override;

protected:
	void Shot(IEnemy* target) override;

private:
	bool isThrust_ = true; // trueなら突き、falseなら薙ぎ払い
	float thrustRange_ = 0.0f;
	float sweepRange_ = 0.0f;
	Vector3 pPos_;
	Vector2 dir_;
};