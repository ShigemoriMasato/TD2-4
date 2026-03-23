#pragma once
#include "../IWeapon.h"
#include <GameObject/Attack/Bullet.h>

class IRangedWeapon : public IWeapon {
public:

	virtual void Initialize(int weaponID, Player::Base* player) override;
	virtual void Update(float deltaTime) override;

protected:

	virtual bool EnemyCheck() override;
	virtual void Shot(IEnemy* target) override;
	void RegisterID(int id) { shotEnemyIDs_.push_back(id); }
	void ClearIDs() { shotEnemyIDs_.clear(); }

	float rate_;
	float range_;
	float speed_;
	float bulletNum_ = 0.0f;
	float penetration_ = 0;
	float spreadAngle_ = 0.0f;

	bool shotIDRegister_ = true;	// 射撃した敵のIDを登録するかどうか

	//あまりにも長いと当たらないので、0.5f固定にして、スピードで調整すること
	const float lifeTime_ = 0.5f;

	float rateTimer_ = 0.0f;

private:

	std::vector<int> shotEnemyIDs_;

};
