#pragma once
#include "IController.h"
#include <../Engine/SHEngine.h>

class InputController : public IController {
public:
	InputController(SHEngine::Input* input) : input_(input) {}

	// 移動方向を返す
	Vector2 GetMoveDirection() override;

	// ダッシュ入力されたか
	bool IsDashTriggered() override;

private:
	SHEngine::Input* input_ = nullptr;
};
