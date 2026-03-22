#include "PlayerAI.h"
#include "GameObject/Distance.h"

Vector3 PlayerAI::ComputeMoveDirection(const Vector3& playerPos, const std::vector<IEnemy*>& enemies) {
	Vector3 targetDir = {0.0f, 0.0f, 0.0f};
	Vector3 escapeDir = {0.0f, 0.0f, 0.0f};
	int escapeCount = 0;

	IEnemy* nearest = nullptr;
	float minDist = FLT_MAX; // floatの最大値

	for (auto* enemy : enemies) {
		if (!enemy->IsActive())
			continue;

		// 一番近い敵を記録
		float dist = Distance(playerPos, enemy->GetDrawInfo().position);
		if (dist < minDist) {
			minDist = dist;
			nearest = enemy;
		}

		// 逃走範囲内にいるすべての敵から反発力を受ける
		if (dist < escapeRange_) {
			// 敵からプレイヤーへ向かうベクトル
			Vector3 dirAway = (playerPos - enemy->GetDrawInfo().position).Normalize();

			// 敵が近いほど強い力で押し返されるように重み付けする
			float weight = 1.0f - (dist / escapeRange_);

			escapeDir.x += dirAway.x * weight;
			escapeDir.y += dirAway.y * weight;
			escapeDir.z += dirAway.z * weight;
			escapeCount++;
		}

		// 最終的な目標方向を決定
		if (escapeCount > 0) {
			// 完全に相殺されて0ベクトルになっていないかチェック
			float escapeLength = std::sqrtf(escapeDir.x * escapeDir.x + escapeDir.y * escapeDir.y + escapeDir.z * escapeDir.z);

			if (escapeLength > 0.001f) {
				targetDir = escapeDir.Normalize();
			} else {
				// 完全に囲まれて力が相殺された場合、まっすぐ進むようにする
				targetDir = {0.0f, 0.0f, 1.0f};
			}
		} else {
			// ターゲットがいる場合はそれを最優先にし、いなければ一番近い敵を対象にする
			IEnemy* target = targetEnemy_ ? targetEnemy_ : nearest;

			if (target) {
				// 距離を計算
				float dist = Distance(playerPos, target->GetDrawInfo().position);

				// ターゲット指定されている場合は少し遠くても追うか、指定がなければ追跡範囲内だけ追う
				if (targetEnemy_ || dist < chaseRange_) {
					targetDir = (target->GetDrawInfo().position - playerPos).Normalize();
				}
			}
		}

		// 移動方向を補間
		float smoothFactor = 0.05f;

		currentDir_.x = currentDir_.x + (targetDir.x - currentDir_.x) * smoothFactor;
		currentDir_.y = currentDir_.y + (targetDir.y - currentDir_.y) * smoothFactor;
		currentDir_.z = currentDir_.z + (targetDir.z - currentDir_.z) * smoothFactor;

		// ベクトルが小さすぎる場合は停止状態にする
		float length = std::sqrtf(currentDir_.x * currentDir_.x + currentDir_.y * currentDir_.y + currentDir_.z * currentDir_.z);
		if (length < 0.01f) {
			return {0.0f, 0.0f, 0.0f};
		}
	}

	return currentDir_.Normalize();
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
