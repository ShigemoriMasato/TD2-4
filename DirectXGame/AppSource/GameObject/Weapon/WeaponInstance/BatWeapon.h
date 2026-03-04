#pragma once
#include "../AppSource/GameObject/Weapon/AttackObject/SlashAttack.h"
#include "../AppSource/GameObject/Weapon/WeaponController.h"

class BatWeapon : public BaseWeapon {
public:
	// コンストラクタ
	BatWeapon(const WeaponData& data) : BaseWeapon(data) {}

	// 更新処理
	void Update(float deltaTime, WeaponController* controller) override;

	// 描画処理
	void Draw(CmdObj* cmdObj) override;

protected:
	// 攻撃オブジェクトの生成
	void Attack(WeaponController* controller);
};
