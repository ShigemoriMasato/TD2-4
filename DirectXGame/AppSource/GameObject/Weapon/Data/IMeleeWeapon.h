#pragma once
#include "../IWeapon.h"
#include <GameObject/Attack/Swing.h>

class IMeleeWeapon : public IWeapon {
public:

	void Initialize(int weaponID, Player::Base* player) override;
	void Update(float deltaTime) override;

protected:

	virtual bool EnemyCheck() override;
	virtual void Shot(IEnemy* target) override;

	float rate_;				//攻撃の速さ(秒)
	float attackRate_;			//連続攻撃の速さ(秒)
	float range_;				//攻撃の届く距離
	float attackNum_ = 0.0f;	//攻撃回数

	//あまりにも長いと当たらないので、0.5f固定にして、スピードで調整すること
	const float lifeTime_ = 0.5f;

	float rateTimer_ = 0.0f;

	bool isAttacking_ = false;
	int attackCount_ = 0;
};

