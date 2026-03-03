#include "WeaponController.h"

void WeaponController::AddWeapon(const WeaponData* data) { 
	// 新しい武器インスタンスをリストに追加
	activeWeapons_.emplace_back(std::make_unique<WeaponInstance>(data)); 
}

void WeaponController::Update(const Vector3& playerPos, float deltaTime) {
	for(auto & weapon : activeWeapons_) {
		weapon->Update(playerPos, deltaTime); // 各武器の更新処理を呼び出す
	}
}
