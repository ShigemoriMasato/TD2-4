#pragma once
#include "../AppSource/GameObject/Weapon/WeaponData.h"
#include "IAttackObject.h"

class SlashAttack : public IAttackObject {
public:
	// 武器データと、発生位置・方向などを受け取って初期化
	SlashAttack(const WeaponData& weaponData /*, Vector3 spawnPos, Vector3 direction*/);

	// 更新処理
	void Update(float deltaTime) override;

	// 描画処理
	void Draw(CmdObj* cmdObj) override;

	bool IsActive() const override {
		if (lifeTime_ <= 0.0f || penetrationCount_ < 0) {
			return false;
		}
		return true;
	}

private:
	float lifeTime_; // 斬撃の残り生存時間
};
