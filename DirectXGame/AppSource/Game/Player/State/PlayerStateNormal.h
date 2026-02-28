#pragma once
#include "IPlayerState.h"

namespace Player {

/// <summary>
/// プレイヤーの通常状態
/// </summary>
class StateNormal : public IPlayerState {
public:
	// 更新処理
	void Update(Base* player, float deltaTime) override;
};

} // namespace Player
