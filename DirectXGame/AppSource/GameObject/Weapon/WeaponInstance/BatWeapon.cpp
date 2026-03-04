#include "BatWeapon.h"

void BatWeapon::Update(float deltaTime, WeaponController* controller) {

}

void BatWeapon::Draw(CmdObj* cmdObj) {

}

void BatWeapon::Attack(WeaponController* controller) {
	// プレイヤーの位置や狙っている方向(最も近い敵の位置)を取得

	// 武器のパラメータを渡して斬撃オブジェクトの生成
	auto slash=std::make_unique<SlashAttack>(data_/*, playerPos, aimDir*/);

	// コントローラーに攻撃オブジェクトを渡し、リストに追加してもらう
	controller->AddAttackObject(std::move(slash));
}