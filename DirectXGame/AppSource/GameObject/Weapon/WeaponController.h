#pragma once
#include <vector>
#include <memory>
#include "WeaponInstance.h"
#include "WeaponData.h"
#include <../Engine/Utility/Vector.h>

class WeaponManager;

/// <summary>
/// 装備中の武器群を管理するクラス
/// </summary>
class WeaponController {
public:
	// 武器の追加
	void AddWeapon(const WeaponData* data);

	// 更新処理(各武器のクールダウンと攻撃実行の管理)
	void Update(const Vector3& playerPos, float deltaTime);

private:
	// アクティブ状態の武器インスタンスのリスト
	std::vector<std::unique_ptr<WeaponInstance>> activeWeapons_;
};
