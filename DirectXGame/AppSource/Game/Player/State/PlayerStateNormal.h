#pragma once
#include "IPlayerState.h"

/// <summary>
/// プレイヤーの通常状態
/// </summary>
class PlayerStateNormal : public IPlayerState {
public:
	// 更新処理
	void Update(Player* player, float deltaTime) override;
};
