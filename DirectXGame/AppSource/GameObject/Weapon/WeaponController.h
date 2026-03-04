#pragma once
#include "../AppSource/GameObject/Weapon/AttackObject/IAttackObject.h"
#include "../AppSource/GameObject/Weapon/WeaponInstance/BaseWeapon.h"
#include "WeaponManager.h"
#include <memory>
#include <vector>

/// <summary>
/// 武器の管理クラス。武器の実体を生成し、攻撃オブジェクトの管理も行う
/// </summary>
class WeaponController {
public:
	// コンストラクタ
	WeaponController(WeaponManager* weaponManager) : weaponManager_(weaponManager) {}

	// IDを受け取ってパラメータに基づく武器の実体を生成&リストに追加
	void EquipWeapon(int weaponID);

	// 武器から攻撃が生成されたときにリストに追加する用の関数
	void AddAttackObject(std::unique_ptr<IAttackObject> attackObj);

	// リスト内の更新処理を呼ぶ
	void Update(float deltaTime);

	// リスト内の描画処理を呼ぶ
	void Draw(CmdObj* cmdObj);

private:
	WeaponManager* weaponManager_ = nullptr;

	// 実体化した武器のリスト
	std::vector<std::unique_ptr<BaseWeapon>> weapons_;

	// 放たれた攻撃(弾、斬撃等)のリスト
	std::vector<std::unique_ptr<IAttackObject>> activeAttacks_;
};
