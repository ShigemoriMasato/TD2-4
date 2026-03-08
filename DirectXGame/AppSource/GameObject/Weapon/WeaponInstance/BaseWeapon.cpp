#include "BaseWeapon.h"

void BaseWeapon::Update(float deltaTime, WeaponController* controller, Matrix4x4 vpMatrix, Vector3 playerPos) {
	attackTimer_ -= deltaTime;

	// 攻撃速度に応じて攻撃処理を呼び出す
	if (attackTimer_ <= 0.0f) {
		Attack(controller);
		attackTimer_ = data_.attackSpeed; // タイマー初期化
	}
}
