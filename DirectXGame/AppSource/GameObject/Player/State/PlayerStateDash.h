#pragma once
#include "IPlayerState.h"

namespace Player {

class Base;

class StateDash : public IPlayerState {
public:
	// 状態切り替え時に一度だけ呼ばれる関数
	void Enter(Base* player) override;

	// 更新処理
	void Update(Base* player, float deltaTime) override;

private:
	// ダッシュ時間を記録する変数
	float dashTimer_ = 0.0f;

	// 残像生成の間隔を計るタイマー
	float spawnTimer_ = 0.0f;
};

} // namespace Player