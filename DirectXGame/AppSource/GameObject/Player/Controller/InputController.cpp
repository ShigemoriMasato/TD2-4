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
	return dir;
}

bool InputController::IsDashTriggered() { return input_->GetKeyState(DIK_SPACE) && !input_->GetPreKeyState(DIK_SPACE); }
