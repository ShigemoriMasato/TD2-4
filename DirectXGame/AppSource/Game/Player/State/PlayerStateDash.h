#pragma once
#include "IPlayerState.h"
class PlayerStateDash : public IPlayerState {
public:
	// 状態切り替え時に一度だけ呼ばれる関数
	void Enter(Player* player) override;

	// 更新処理
	void Update(Player* player, float deltaTime) override;

private:
	// ダッシュ時間を記録する変数
	float dashTimer_ = 0.0f;
};
