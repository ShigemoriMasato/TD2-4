#pragma once
#include <../Engine/Utility/Vector.h>
#include "GameObject/Map/MapInfo.h"

class IController {
public:
	virtual ~IController() = default;

	// 移動方向を返す
	virtual Vector2 GetMoveDirection(float deltaTime, const MapInfo& mapInfo) = 0;

	// ダッシュが入力されたか
	virtual bool IsDashTriggered() = 0;

	// ターゲット位置を設定する（クリック移動用など）
	virtual void SetTargetPosition(const Vector3& pos) {}

	// ターゲットが存在するかどうか
	virtual bool HasTarget() const { return false; }

	// ターゲットの座標を取得する
	virtual Vector3 GetTargetPosition() const { return {}; }

	virtual void DrawImGui() = 0;
};