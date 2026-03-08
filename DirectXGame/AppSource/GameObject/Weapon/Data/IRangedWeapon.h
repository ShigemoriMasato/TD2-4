#pragma once
#include "../IWeapon.h"

class IRangedWeapon : public IWeapon {
public:

	virtual void Initialize(int weaponID, Player::Base* player) override;
	virtual void Update(float deltaTime) override;

protected:

	bool Shot();

	float rate_;
	float range_;
	float speed_;
	float bulletNum_ = 0.0f;
	float penetration_ = 0;
	float spreadAngle_ = 0.0f;

	//あまりにも長いと当たらないので、0.5f固定にして、スピードで調整すること
	const float lifeTime_ = 0.5f;

	IAttackObject::Config config_;

	float rateTimer_ = 0.0f;
};
