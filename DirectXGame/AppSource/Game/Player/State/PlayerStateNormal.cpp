#include "PlayerStateNormal.h"
#include "../AppSource/Game/Player/Player.h"
#include "PlayerStateDash.h"
#include <cmath>

void PlayerStateNormal::Update(Player* player, float deltaTime) {
	// クールダウンの更新
	player->UpdateDashCooldown();

	// 移動入力の取得
	Vector2 dir = {0.0f, 0.0f};
	auto input = player->GetInput();
	if (input->GetKeyState(DIK_W))
		dir.y += 1.0f;
	if (input->GetKeyState(DIK_S))
		dir.y -= 1.0f;
	if (input->GetKeyState(DIK_D))
		dir.x += 1.0f;
	if (input->GetKeyState(DIK_A))
		dir.x -= 1.0f;

	// 正規化
	float length = std::sqrt(dir.x * dir.x + dir.y * dir.y);
	if (length > 0.0f) {
		dir.x /= length;
		dir.y /= length;
	}

	// ダッシュのトリガー判定
	if (input->GetKeyState(DIK_SPACE) && !input->GetPreKeyState(DIK_SPACE) && player->CanDash() && length > 0.0f) {
		// ダッシュ方向をPlayerにセットして、状態をダッシュに切り替える
		player->SetDashDir(dir);
		player->ChangeState(std::make_unique<PlayerStateDash>());
		return; // 状態が変わったのでこのフレームの処理は終了
	}

	// 通常移動の適用
	Transform& t = player->GetTransform();
	t.position.x += dir.x * player->GetVelocity() * deltaTime;
	t.position.z += dir.y * player->GetVelocity() * deltaTime;
}
