#include "PlayerStateNormal.h"
#include "../AppSource/Game/Player/Player.h"
#include "PlayerStateDash.h"
#include <cmath>
#include <numbers>

using namespace Player;

void StateNormal::Update(Base* player, float deltaTime) {
	// クールダウンの更新
	player->UpdateDashCooldown(deltaTime);

	Vector3& rotate = player->GetTransform().rotate;

	// 移動入力の取得
	Vector2 dir = {0.0f, 0.0f};
	auto input = player->GetInput();
	if (input->GetKeyState(DIK_W)) {
		dir.y += 1.0f;
	}
	if (input->GetKeyState(DIK_S)) {
		dir.y -= 1.0f;
	}
	if (input->GetKeyState(DIK_D)) {
		dir.x += 1.0f;
	}
	if (input->GetKeyState(DIK_A)) {
		dir.x -= 1.0f;
	}

	// 正規化
	float length = std::sqrt(dir.x * dir.x + dir.y * dir.y);
	if (length > 0.0f) {
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
	}

	// ダッシュのトリガー判定
	if (input->GetKeyState(DIK_SPACE) && !input->GetPreKeyState(DIK_SPACE) && player->CanDash() && length > 0.0f) {
		// ダッシュ方向をPlayerにセットして、状態をダッシュに切り替える
		player->SetDashDir(dir);
		player->ChangeState(std::make_unique<Player::StateDash>());
		return; // 状態が変わったのでこのフレームの処理は終了
	}

	// 通常移動の適用
	Transform& t = player->GetTransform();
	t.position.x += dir.x * player->GetVelocity() * deltaTime;
	t.position.z += dir.y * player->GetVelocity() * deltaTime;

	// プレイヤーがステージ外に出ないようにする
	ClampPosition(player);
}

void Player::StateNormal::ClampPosition(Base* player) {
	// プレイヤーがステージ買いに出ないようにする
	float posX = std::clamp(player->GetTransform().position.x, -19.0f, 19.0f);
	float posZ = std::clamp(player->GetTransform().position.z, -19.0f, 19.0f);

	player->SetPosition(Vector3(posX, 0.0f, posZ));
}
