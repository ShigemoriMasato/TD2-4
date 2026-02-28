#include "PlayerStateDash.h"
#include "../AppSource/Game/Player/Player.h"
#include "PlayerStateNormal.h"

using namespace Player;

void StateDash::Enter(Base* player) {
	// ダッシュ開始時の設定
	dashTimer_ = player->GetDashDuration();
}

void StateDash::Update(Base* player, float deltaTime) {
	Vector2 dir = player->GetDashDir();    // ダッシュ方向
	Transform& t = player->GetTransform(); // プレイヤーのSRT

	// ダッシュ処理
	t.position.x += dir.x * player->GetDashSpeed() * deltaTime;
	t.position.z += dir.y * player->GetDashSpeed() * deltaTime;

	// 残像の生成処理
	//spawnTimer_ -= deltaTime;
	//if (spawnTimer_ <= 0.0f) {
	//	player->SpawnAfterImage();
	//	spawnTimer_ = 0.03f; // 残像の生成間隔を設定
	//}

	// タイマーの更新
	dashTimer_ -= deltaTime;
	if (dashTimer_ <= 0.0f) {
		// ダッシュ終了時、クールダウンを開始して通常状態に戻る
		player->StartDashCooldown();
		player->ChangeState(std::make_unique<StateNormal>());
	}
}
