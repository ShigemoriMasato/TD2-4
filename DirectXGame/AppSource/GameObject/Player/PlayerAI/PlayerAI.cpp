#include "PlayerAI.h"
#include "GameObject/Distance.h"
#include "GameObject/Random/Random.h"
#include <../externals/imgui/imgui.h>

Vector3 PlayerAI::ComputeMoveDirection(const Vector3& playerPos, const std::vector<IEnemy*>& enemies, float deltaTime) {
	// ターゲットの敵が倒されていたらターゲットを解除する
	if (targetEnemy_ && !targetEnemy_->IsActive()) {
		targetEnemy_ = nullptr;
	}

	// タイマーが切れている場合のみ、新しい目標方向を考える
	if (moveTimer_ <= 0.0f) {
		Vector3 targetDir = {0.0f, 0.0f, 0.0f};

		// モード別の移動方向決定ロジック
		switch (currentMode_) {
		case AIMode::AGGRESSIVE: {
			// 敵に突っ込み続けるモード
			IEnemy* nearest = FindNearestEnemy(playerPos, enemies);
			if (nearest) {
				targetDir = (nearest->GetPosition() - playerPos).Normalize();
			}
			break;
		}
		case AIMode::EVASIVE: {
			// 敵から逃げ続けるモード
			Vector3 escapeDir = {0.0f, 0.0f, 0.0f};
			for (auto* enemy : enemies) {
				if (!enemy->IsActive())
					continue;

				// 全ての敵から距離に応じた反発力を受ける
				float dist = Distance(playerPos, enemy->GetPosition());
				Vector3 dirAway = (playerPos - enemy->GetPosition()).Normalize();

				// 距離が近いほど強く反発する
				float weight = 10.0f / (dist + 1.0f);
				escapeDir.x += dirAway.x * weight;
				escapeDir.y += dirAway.y * weight;
				escapeDir.z += dirAway.z * weight;
			}

			float escapeLength = std::sqrtf(escapeDir.x * escapeDir.x + escapeDir.y * escapeDir.y + escapeDir.z * escapeDir.z);
			if (escapeLength > 0.001f) {
				targetDir = escapeDir.Normalize();
			}
			break;
		}
		case AIMode::SKIRMISH: {
			// 逃げながらもちょっかいをかけるモード
			Vector3 escapeDir = {0.0f, 0.0f, 0.0f};
			int escapeCount = 0;
			IEnemy* nearest = nullptr;
			float minDist = FLT_MAX;

			for (auto* enemy : enemies) {
				if (!enemy->IsActive())
					continue;

				float dist = Distance(playerPos, enemy->GetPosition());
				if (dist < minDist) {
					minDist = dist;
					nearest = enemy;
				}

				if (dist < escapeRange_) {
					Vector3 dirAway = (playerPos - enemy->GetPosition()).Normalize();
					float weight = 1.0f - (dist / escapeRange_);
					escapeDir.x += dirAway.x * weight;
					escapeDir.y += dirAway.y * weight;
					escapeDir.z += dirAway.z * weight;
					escapeCount++;
				}
			}

			if (targetEnemy_) {
				targetDir = (targetEnemy_->GetPosition() - playerPos).Normalize();
			} else if (escapeCount > 0) {
				float escapeLength = std::sqrtf(escapeDir.x * escapeDir.x + escapeDir.y * escapeDir.y + escapeDir.z * escapeDir.z);
				if (escapeLength > 0.001f) {
					targetDir = escapeDir.Normalize();
				} else {
					targetDir = {0.0f, 0.0f, 1.0f};
				}
			} else if (nearest && minDist < chaseRange_) {
				targetDir = (nearest->GetPosition() - playerPos).Normalize();
			}
			break;
		}
		}

		// 新しい目標方向をロックする
		lockedTargetDir_ = targetDir;

		// タイマーの設定
		moveTimer_ = RandomUtils::RangeFloat(minTime_, maxTime_);
	}

	// タイマーを減らす
	moveTimer_ -= deltaTime;

	// ロックされている目標方向に向けて、現在の向きを補間
	float smoothFactor = 0.05f;
	currentDir_.x = currentDir_.x + (lockedTargetDir_.x - currentDir_.x) * smoothFactor;
	currentDir_.y = currentDir_.y + (lockedTargetDir_.y - currentDir_.y) * smoothFactor;
	currentDir_.z = currentDir_.z + (lockedTargetDir_.z - currentDir_.z) * smoothFactor;

	// ベクトルが小さすぎる場合は停止状態にする
	float length = std::sqrtf(currentDir_.x * currentDir_.x + currentDir_.y * currentDir_.y + currentDir_.z * currentDir_.z);
	if (length < 0.01f) {
		return {0.0f, 0.0f, 0.0f};
	}

	return currentDir_.Normalize();
}

IEnemy* PlayerAI::FindNearestEnemy(const Vector3& playerPos, const std::vector<IEnemy*>& enemies) {
	IEnemy* nearest = nullptr;
	float minDist = FLT_MAX;

	for (auto* enemy : enemies) {
		if (!enemy->IsActive())
			continue;

		// 距離を求める
		float dist = Distance(playerPos, enemy->GetPosition());
		if (dist < minDist) {
			minDist = dist;
			nearest = enemy;
		}
	}
	return nearest; // プレイヤーに最も近い敵を返す
}

void PlayerAI::DrawImGui() {
#ifdef USE_IMGUI
	ImGui::Begin("プレイヤーAI");

	// モード切り替え用のボタン
	int mode = static_cast<int>(currentMode_);
	ImGui::Text("AIモード");
	ImGui::RadioButton("突撃", &mode, 0);
	ImGui::RadioButton("逃走", &mode, 1);
	ImGui::RadioButton("遊撃", &mode, 2);
	currentMode_ = static_cast<AIMode>(mode);

	ImGui::Separator();

	if (currentMode_ == AIMode::SKIRMISH) {
		ImGui::DragFloat("Chase Range", &chaseRange_, 0.1f, 0.0f, 50.0f);
		ImGui::DragFloat("Escape Range", &escapeRange_, 0.1f, 0.0f, 50.0f);
	}

	ImGui::Text("タイマーの乱数");
	ImGui::DragFloat("最小時間", &minTime_, 0.01f);
	ImGui::DragFloat("最大時間", &maxTime_, 0.01f);

	ImGui::End();
#endif
}