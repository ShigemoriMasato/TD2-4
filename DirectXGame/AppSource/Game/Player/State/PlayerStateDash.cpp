#include "PlayerStateDash.h"
#include "../AppSource/Game/Player/Player.h"
#include "PlayerStateNormal.h"

void PlayerStateDash::Enter(Player* player) {
	// ダッシュ開始時の設定
	dashTimer_ = player->GetDashDuration();
}

void PlayerStateDash::Update(Player* player, float deltaTime) {
	Vector2 dir = player->GetDashDir();    // ダッシュ方向
	Transform& t = player->GetTransform(); // プレイヤーのSRT

	// ダッシュ処理
	t.position.x += dir.x * player->GetDashSpeed() * deltaTime;
	t.position.z += dir.y * player->GetDashSpeed() * deltaTime;

	// タイマーの更新
	dashTimer_ -= deltaTime;
	if (dashTimer_ <= 0.0f) {
		// ダッシュ終了時、クールダウンを開始して通常状態に戻る
		player->StartDashCooldown();
		player->ChangeState(std::make_unique<PlayerStateNormal>());
	}
}
