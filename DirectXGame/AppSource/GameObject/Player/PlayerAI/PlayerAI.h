#pragma once
#include <../Engine/Utility/Vector.h>
#include <GameObject/Enemy/IEnemy.h>

class PlayerAI {
public:
	// 敵のリストから移動方向を求める
	Vector3 ComputeMoveDirection(const Vector3& playerPos, const std::vector<IEnemy*>& enemies);

private:
	// プレイヤーに最も近い敵を求める
	IEnemy* FindNearestEnemy(const Vector3& playerPos, const std::vector<IEnemy*>& enemies);

private:
	// 追跡開始距離
	float chaseRange_ = 20.0f;

	// 闘争開始距離
	float escapeRange_ = 5.0f;
};
