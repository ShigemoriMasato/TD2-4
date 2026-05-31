#include "ShotGun.h"
#include "GameObject/Attack/PistolBullet.h"

void ShotGun::Shot(IEnemy* target) {
	// 敵の方向に弾を生成
	isAnimation_ = true;
	for (int i = 0; i < shots_; ++i) {
		std::unique_ptr<PistolBullet> bullet = std::make_unique<PistolBullet>();
		bullet->Initialize(config_);
		attackManager_->AddObj(std::move(bullet));
	}
}
