#include "AIController.h"

Vector2 AIController::GetMoveDirection(float deltaTime) {
	// 現在の敵リストを取得
	std::vector<IEnemy*> enemies = enemyManager_->GetEnemies();

	// AIでの移動方向を計算
	Vector3 dir = ai_->ComputeMoveDirection(*playerPos_, enemies, deltaTime);

	// 侵攻方向を返す
	return {dir.x, dir.z};
}

bool AIController::IsDashTriggered() { return false; }

void AIController::DrawImGui(){
	//#ifdef USE_IMGUI
	ai_->DrawImGui();
	//#endif
}
