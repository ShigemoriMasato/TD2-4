#pragma once
#include <GameObject/Attack/AttackManager.h>
#include <GameObject/Enemy/EnemyManager.h>
#include <GameObject/Weapon/WeaponDatabase.h>
#include <GameObject/Player/Player.h>

class Piece;

class IWeapon {
public:

	static void StaticInitialize(AttackManager* attackManager, EnemyManager* enemyManager, WeaponDatabase* weaponManager) { attackManager_ = attackManager, enemyManager_ = enemyManager, weaponManager_ = weaponManager; }

	virtual void Initialize(int weaponID, Player::Base* player);
	virtual void Update(float deltaTime);

	virtual float GetDirection() const { return config_.direction; }

	WeaponData* GetWeaponData() {return weaponData_;}

	void SetPiecePtr(Piece* piece) { piece_ = piece; }
	Piece* GetPiecePtr() const { return piece_; }

	bool GetIsAnimation() const { return isAnimation_;}

protected:

	static inline AttackManager* attackManager_ = nullptr;
	static inline EnemyManager* enemyManager_ = nullptr;
	static inline WeaponDatabase* weaponManager_ = nullptr;

	IAttackObject::Config config_;

	WeaponData* weaponData_;
	Player::Base* player_;

	Piece* piece_ = nullptr;

	bool isAnimation_ = false;

};
