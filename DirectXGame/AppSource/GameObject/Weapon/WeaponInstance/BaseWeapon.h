#pragma once
#include "../AppSource/GameObject/Weapon/AttackObject/IAttackObject.h"
#include "../AppSource/GameObject/Weapon/WeaponData.h"
#include <memory>

class WeaponController;

/// <summary>
/// 武器のパラメータを保持し、武器自体の描画や攻撃オブジェクトの生成を管理するクラス
/// </summary>
class BaseWeapon {
public:
	// コンストラクタ
	BaseWeapon(const WeaponData& data) : data_(data), attackTimer_(0.0f) {}

	virtual ~BaseWeapon() = default;

	// 武器固有の更新処理
	virtual void Update(float deltaTime, WeaponController* controller, Matrix4x4 vpMatrix, Vector3 playerPos);

	// 武器の描画
	virtual void Draw(CmdObj* cmdObj) = 0;

protected:
	// 実際の攻撃処理
	virtual void Attack(WeaponController* controller) = 0;

protected:
	WeaponData data_; // 武器のパラメータデータ
	float attackTimer_;
};
