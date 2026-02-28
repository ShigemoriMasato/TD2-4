#pragma once

namespace Player {
class Base;

/// <summary>
/// プレイヤーの状態の基底クラス
/// </summary>
class IPlayerState {
public:
	virtual ~IPlayerState() = default;

	// 状態切り替え時に一度だけ呼ばれる関数
	virtual void Enter(Base* player) {};

	// 更新処理
	virtual void Update(Base* player, float deltaTime) = 0;

	// 別の状態に切り替わる直前に呼ばれる
	virtual void Exit(Base* player) {};
};

} // namespace Base