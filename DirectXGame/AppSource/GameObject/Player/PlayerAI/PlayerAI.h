#pragma once
#include <../Engine/Utility/Vector.h>
#include <GameObject/Enemy/IEnemy.h>

class PlayerAI {
public:
	// 敵のリストから移動方向を求める
	Vector3 ComputeMoveDirection(const Vector3& playerPos, const std::vector<IEnemy*>& enemies);

	// ターゲットの設定
	void SetTargetEnemy(IEnemy* target) { targetEnemy_ = target; }

	// ターゲットの取得
	IEnemy* GetTargetEnemy() const { return targetEnemy_; }

private:
	// プレイヤーに最も近い敵を求める
	IEnemy* FindNearestEnemy(const Vector3& playerPos, const std::vector<IEnemy*>& enemies);

private:
	// 追跡開始距離
	float chaseRange_ = 15.0f;

	// 逃走開始距離
	float escapeRange_ = 5.0f;

	// 直前のフレームの移動方向
	Vector3 currentDir_ = {0.0f, 0.0f, 0.0f};

	// 現在ターゲットにしている敵
	IEnemy* targetEnemy_ = nullptr;
};
