#include "InputController.h"

Vector2 InputController::GetMoveDirection() {
	Vector2 dir = {0.0f, 0.0f};
	if (input_->GetKeyState(DIK_W))
		dir.y += 1.0f;
	if (input_->GetKeyState(DIK_S))
		dir.y -= 1.0f;
	if (input_->GetKeyState(DIK_D))
		dir.x += 1.0f;
	if (input_->GetKeyState(DIK_A))
		dir.x -= 1.0f;
		
	// キー入力があればターゲット移動をキャンセル
	if (dir.x != 0.0f || dir.y != 0.0f) {
		hasTarget_ = false;
	} else if (hasTarget_ && currentPos_) {
		Vector3 diff = {targetPos_.x - currentPos_->x, 0.0f, targetPos_.z - currentPos_->z};
		float dist = std::sqrt(diff.x * diff.x + diff.z * diff.z);
		if (dist > 0.1f) {
			dir.x = diff.x / dist;
			dir.y = diff.z / dist;
		} else {
			hasTarget_ = false; // 到達したら解除
		}
	}

	// 入力もターゲット移動もない場合はAIに任せる
	if (dir.x == 0.0f && dir.y == 0.0f && !hasTarget_ && fallbackController_) {
		return fallbackController_->GetMoveDirection();
	}

	return dir;
}

bool InputController::IsDashTriggered() { return input_->GetKeyState(DIK_SPACE) && !input_->GetPreKeyState(DIK_SPACE); }
