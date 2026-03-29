#include "PlayerAI.h"
#include "GameObject/Distance.h"
#include "GameObject/Random/Random.h"
#include <../externals/imgui/imgui.h>
#include <algorithm>

Vector3 PlayerAI::ComputeMoveDirection(const Vector3& playerPos, const std::vector<IEnemy*>& enemies, const MapInfo& mapInfo, float deltaTime) {
	// ターゲットの敵が倒されていたらターゲットを解除する
	if (targetEnemy_ && !targetEnemy_->IsActive()) {
		targetEnemy_ = nullptr;
	}

	// 敵が至近距離にいるかチェック
	bool isDanger = false;
	if (currentMode_ != AIMode::AGGRESSIVE) {
		for (auto* enemy : enemies) {
			if (enemy->IsActive() && Distance(playerPos, enemy->GetPosition()) < dangerDist_) {
				isDanger = true;
				break;
			}
		}
	}

	// 危険状態になった瞬間だけタイマーを強制ゼロにして即時反応させる
	if (isDanger && moveTimer_ > 0.2f) {
		moveTimer_ = 0.0f;
	}

	// タイマーが切れた場合のみ新しい目標方向を考える
	if (moveTimer_ <= 0.0f) {
		Vector3 targetDir = {0.0f, 0.0f, 0.0f};

		// モード別の移動方向決定ロジック
		switch (currentMode_) {
		case AIMode::AGGRESSIVE: {
			IEnemy* nearest = FindNearestEnemy(playerPos, enemies);
			if (nearest) {
				targetDir = (nearest->GetPosition() - playerPos).Normalize();
			}
			break;
		}
		case AIMode::EVASIVE: {
			Vector3 escapeDir = {0.0f, 0.0f, 0.0f};
			for (auto* enemy : enemies) {
				if (!enemy->IsActive())
					continue;

				float dist = Distance(playerPos, enemy->GetPosition());

				// 完全に重なっている場合のゼロベクトル対策
				Vector3 dirAway = playerPos - enemy->GetPosition();
				if (dist < 0.001f) {
					// 重なっている場合はランダムな方向に逃げるように少しずらす
					dirAway = {RandomUtils::RangeFloat(-1.0f, 1.0f), 0.0f, RandomUtils::RangeFloat(-1.0f, 1.0f)};
					if (dirAway.x == 0.0f && dirAway.z == 0.0f)
						dirAway.x = 1.0f;
				}
				dirAway = dirAway.Normalize();

				float weight = 10.0f / (dist + 1.0f);
				escapeDir.x += dirAway.x * weight;
				escapeDir.y += dirAway.y * weight;
				escapeDir.z += dirAway.z * weight;
			}

			// 壁の反発力が無限大になってピンボール化するのを防ぐ
			float distMinX = playerPos.x - mapInfo.minX;
			float distMaxX = mapInfo.maxX - playerPos.x;
			float distMinZ = playerPos.z - mapInfo.minZ;
			float distMaxZ = mapInfo.maxZ - playerPos.z;

			// 力が大きくなりすぎるのを防ぐ
			float minClamp = 0.5f;
			if (distMinX < wallMargin_)
				escapeDir.x += wallRepulsion_ / std::max(distMinX, minClamp);
			if (distMaxX < wallMargin_)
				escapeDir.x -= wallRepulsion_ / std::max(distMaxX, minClamp);
			if (distMinZ < wallMargin_)
				escapeDir.z += wallRepulsion_ / std::max(distMinZ, minClamp);
			if (distMaxZ < wallMargin_)
				escapeDir.z -= wallRepulsion_ / std::max(distMaxZ, minClamp);

			float escapeLength = std::sqrtf(escapeDir.x * escapeDir.x + escapeDir.y * escapeDir.y + escapeDir.z * escapeDir.z);
			if (escapeLength > 0.001f) {
				targetDir = escapeDir.Normalize();
			}
			break;
		}
		case AIMode::SKIRMISH: {
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
					// 完全に重なっている場合のゼロベクトル対策
					Vector3 dirAway = playerPos - enemy->GetPosition();
					if (dist < 0.001f) {
						dirAway = {RandomUtils::RangeFloat(-1.0f, 1.0f), 0.0f, RandomUtils::RangeFloat(-1.0f, 1.0f)};
						if (dirAway.x == 0.0f && dirAway.z == 0.0f)
							dirAway.x = 1.0f;
					}
					dirAway = dirAway.Normalize();

					float weight = 1.0f - (dist / escapeRange_);
					escapeDir.x += dirAway.x * weight;
					escapeDir.y += dirAway.y * weight;
					escapeDir.z += dirAway.z * weight;
					escapeCount++;
				}
			}

			// 壁の反発力が無限大になってピンボール化するのを防ぐ
			float distMinX = playerPos.x - mapInfo.minX;
			float distMaxX = mapInfo.maxX - playerPos.x;
			float distMinZ = playerPos.z - mapInfo.minZ;
			float distMaxZ = mapInfo.maxZ - playerPos.z;
			float minClamp = 0.5f;

			if (distMinX < wallMargin_)
				escapeDir.x += wallRepulsion_ / std::max(distMinX, minClamp);
			if (distMaxX < wallMargin_)
				escapeDir.x -= wallRepulsion_ / std::max(distMaxX, minClamp);
			if (distMinZ < wallMargin_)
				escapeDir.z += wallRepulsion_ / std::max(distMinZ, minClamp);
			if (distMaxZ < wallMargin_)
				escapeDir.z -= wallRepulsion_ / std::max(distMaxZ, minClamp);

			float escapeLength = std::sqrtf(escapeDir.x * escapeDir.x + escapeDir.y * escapeDir.y + escapeDir.z * escapeDir.z);
			if (escapeCount > 0 || escapeLength > 0.001f) {
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

		// 危険状態の時は短いスパンで、安全な時は通常の乱数時間で方向転換
		moveTimer_ = isDanger ? 0.2f : RandomUtils::RangeFloat(minTime_, maxTime_);
	}

	// タイマーを減らす
	moveTimer_ -= deltaTime;

	// ロックされている目標方向に向けて、現在の向きを補間
	float smoothFactor = isDanger ? 0.15f : 0.05f;
	currentDir_.x += (lockedTargetDir_.x - currentDir_.x) * smoothFactor;
	currentDir_.y += (lockedTargetDir_.y - currentDir_.y) * smoothFactor;
	currentDir_.z += (lockedTargetDir_.z - currentDir_.z) * smoothFactor;

	// 振り向き時にベクトルが相殺されて一瞬止まる減少の防止
	float length = std::sqrtf(currentDir_.x * currentDir_.x + currentDir_.y * currentDir_.y + currentDir_.z * currentDir_.z);
	if (length < 0.01f) {
		// 相殺された場合は、強制的に目標方向を向かせる
		currentDir_ = lockedTargetDir_;

		// 目標方向自体がない場合は停止
		if (std::sqrtf(currentDir_.x * currentDir_.x + currentDir_.y * currentDir_.y + currentDir_.z * currentDir_.z) < 0.01f) {
			return {0.0f, 0.0f, 0.0f};
		}
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

	ImGui::Separator();
	ImGui::Text("回避設定");
	ImGui::DragFloat("緊急回避距離", &dangerDist_, 0.1f, 0.0f, 20.0f);
	ImGui::DragFloat("壁の検知距離", &wallMargin_, 0.1f, 0.0f, 20.0f);
	ImGui::DragFloat("壁の反発力", &wallRepulsion_, 0.5f, 0.0f, 100.0f);

	ImGui::End();
#endif
}