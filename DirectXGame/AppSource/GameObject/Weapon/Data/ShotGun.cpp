#include "ShotGun.h"

void ShotGun::Shot(IEnemy* target) {
	// 敵の方向に弾を生成
	isAnimation_ = true;
	for (int i = 0; i < shots_; ++i) {
		std::unique_ptr<Bullet> bullet = std::make_unique<Bullet>();
		bullet->Initialize(config_);
		attackManager_->AddObj(std::move(bullet));
	}
}
