#pragma once
#include <../Engine/Utility/Vector.h>

class IController {
public:
	virtual ~IController() = default;

	// 移動方向を返す
	virtual Vector2 GetMoveDirection() = 0;

	// ダッシュが入力されたか
	virtual bool IsDashTriggered() = 0;
};