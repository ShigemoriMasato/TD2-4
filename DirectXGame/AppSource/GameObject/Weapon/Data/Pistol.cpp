#include "Pistol.h"
#include <GameObject/Attack/Bullet.h>

void Pistol::Initialize(int weaponID, Player::Base* player) {
	IWeapon::Initialize(weaponID, player);
	// ピストルの固有初期化
	// 例えば、弾のサイズや発射レートなどを設定
	size_ = 1.0f; // Playerのステータス的なsize。発射速度or生存時間に影響させる
	rate_ = 2.0f; // 発射レート1発あたりのクールダウン時間
}

void Pistol::Update(float deltaTime) {
	auto enemies = enemyManager_->GetEnemies();
	rateTimer_ += deltaTime;

	if (rateTimer_ < rate_) {
		return;
	}

	// 発射レートに基づいて攻撃を生成
	for (const auto& enemy : enemies) {
		auto drawInfo = enemy->GetDrawInfo();
		Vector3 ePos = drawInfo.position;
		Vector3 pPos = *playerPos_;

		float distance = (ePos - pPos).Length();
		constexpr float lifeTime = 0.5f; // 弾の寿命
		float speed = size_ * 30.0f; // 弾の速度
		//一定距離内に敵がいた場合
		if (distance <= lifeTime * speed) {
			auto bullet = std::make_unique<Bullet>();
			bullet->Initialize(size_);
			bullet->SetConfig({ pPos.x, pPos.z }, { ePos.x - pPos.x, ePos.z - pPos.z }, size_ * 30.0f);
			attackManager_->AddObj(std::move(bullet));
			rateTimer_ = 0.0f; // クールダウンリセット
			break;
		}
	}
}
