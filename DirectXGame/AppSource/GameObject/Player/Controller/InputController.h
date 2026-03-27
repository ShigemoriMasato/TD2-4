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

	void SetTargetPosition(const Vector3& pos) override { targetPos_ = pos; hasTarget_ = true; }
	void SetCurrentPosition(Vector3* pos) { currentPos_ = pos; }

	bool HasTarget() const override { return hasTarget_; }
	Vector3 GetTargetPosition() const override { return targetPos_; }

	void SetFallbackController(IController* ai) { fallbackController_ = ai; }

private:
	SHEngine::Input* input_ = nullptr;
	Vector3* currentPos_ = nullptr;
	Vector3 targetPos_ = {};
	bool hasTarget_ = false;

	IController* fallbackController_ = nullptr;
};
