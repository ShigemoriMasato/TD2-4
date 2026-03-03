#pragma once
#include "WeaponData.h"
#include <../Engine/Utility/Vector.h>

class WeaponInstance {
public:
	// コンストラクタ
	WeaponInstance(const WeaponData* data) : weaponData_(data) {
		cooldownDuration_ = weaponData_->attackSpeed; // 攻撃速度をクールダウンの初期値に設定
		currentCooldown_ = 0.0f;                      // クールダウンタイマーを初期化
	}

	// 更新処理
	void Update(const Vector3& playerPos, float deltaTime);

private:
	// プレイヤーに最も近い敵を攻撃する処理
	void ExecuteAttack(const Vector3& playerPos);

private:
	const WeaponData* weaponData_; // 武器の基本データへのポインタ
	float currentCooldown_;        // 現在のクールダウンタイマー
	float cooldownDuration_;       // クールダウンの持続時間
};
