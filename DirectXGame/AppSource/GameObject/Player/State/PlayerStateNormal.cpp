#include "PlayerStateNormal.h"
#include "../AppSource/GameObject/Player/Player.h"
#include "PlayerStateDash.h"
#include <cmath>
#include <numbers>

using namespace Player;

void StateNormal::Update(Base* player, float deltaTime) {
	Vector3& rotate = player->GetTransform().rotate;

	// 移動入力の取得
	Vector2 dir = player->GetController()->GetMoveDirection(deltaTime);

	// 正規化
	float length = std::sqrt(dir.x * dir.x + dir.y * dir.y);
	
	// 入力があるかどうかの判定
	bool isMoving = (length > 0.0f);
	if (isMoving) {
		dir.x /= length;
		dir.y /= length;

		// 目標の角度を求める
		float targetAngle = std::atan2(-dir.x, -dir.y);

		// 現在の角度と目標の角度の差分
		float diff = targetAngle - rotate.y;

		// 差分を収める
		const float kPi = std::numbers::pi_v<float>;
		while (diff < -kPi)
			diff += 2.0f * kPi;
		while (diff > kPi)
			diff -= 2.0f * kPi;

		// 回転を補間
		rotate.y += diff * player->GetRotationSpeed() * deltaTime;
	
		// 通常移動の適用
		Transform& t = player->GetTransform();
		t.position.x += dir.x * player->GetVelocity() * deltaTime;
		t.position.z += dir.y * player->GetVelocity() * deltaTime;
	}

	player->UpdateWalkAnimation(deltaTime, isMoving);
}

void Player::StateNormal::ClampPosition(Base* player) {
	// プレイヤーがステージ買いに出ないようにする
	float posX = std::clamp(player->GetTransform().position.x, -19.0f, 19.0f);
	float posZ = std::clamp(player->GetTransform().position.z, -19.0f, 19.0f);

	player->SetPosition(Vector3(posX, 0.0f, posZ));
}
