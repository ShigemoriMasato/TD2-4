#include "PlayerAI.h"
#include "GameObject/Distance.h"

Vector3 PlayerAI::ComputeMoveDirection(const Vector3& playerPos, const std::vector<IEnemy*>& enemies) {
	IEnemy* nearest = FindNearestEnemy(playerPos, enemies);
	if (!nearest)
		return {0.0f, 0.0f, 0.0f};

	// プレイヤーとプレイヤーに最も近い敵の距離を求める
	float dist = Distance(playerPos, nearest->GetDrawInfo().position);

	// 逃走
	if (dist < escapeRange_) {
		return (playerPos - nearest->GetDrawInfo().position).Normalize();
	}

	// 追跡
	if (dist < chaseRange_) {
		return (nearest->GetDrawInfo().position - playerPos).Normalize();
	}

	// 何もしない
	return {0.0f, 0.0f, 0.0f};
}

IEnemy* PlayerAI::FindNearestEnemy(const Vector3& playerPos, const std::vector<IEnemy*>& enemies) {
	IEnemy* nearest = nullptr;
	float minDist = FLT_MAX; // floatの最大値

	for (auto* enemy : enemies) {
		if (!enemy->IsActive())
			continue;

		// 距離を求める
		float dist = Distance(playerPos, enemy->GetDrawInfo().position);
		if (dist < minDist) {
			minDist = dist;
			nearest = enemy;
		}
	}
	return nearest; // プレイヤーに最も近い敵を返す
}
