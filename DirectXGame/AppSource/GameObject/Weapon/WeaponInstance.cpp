#include "WeaponInstance.h"

void WeaponInstance::Update(const Vector3& playerPos, float deltaTime){
	// クールダウンタイマーを更新
	currentCooldown_ -= deltaTime;

	if (currentCooldown_ <= 0.0f) {
		// クールダウンが終了したら攻撃を実行
		ExecuteAttack(playerPos);
		currentCooldown_ = cooldownDuration_; // クールダウンをリセット
	}
}

void WeaponInstance::ExecuteAttack(const Vector3& playerPos){

}