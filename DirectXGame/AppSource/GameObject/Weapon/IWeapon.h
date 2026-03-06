#pragma once
#include <GameObject/Attack/AttackManager.h>
#include <GameObject/Enemy/EnemyManager.h>
#include <GameObject/Weapon/WeaponDatabase.h>
#include <GameObject/Player/Player.h>

class IWeapon {
public:

	static void StaticInitialize(AttackManager* attackManager, EnemyManager* enemyManager, WeaponDatabase* weaponManager) { attackManager_ = attackManager, enemyManager_ = enemyManager, weaponManager_ = weaponManager; }

	virtual void Initialize(int weaponID, Player::Base* player);
	virtual void Update(float deltaTime);

protected:

	static inline AttackManager* attackManager_ = nullptr;
	static inline EnemyManager* enemyManager_ = nullptr;
	static inline WeaponDatabase* weaponManager_ = nullptr;

	WeaponData* weaponData_;
	Vector3* playerPos_ = nullptr;

};
