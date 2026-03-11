#include "ShotGun.h"

void ShotGun::Initialize(int weaponID, Player::Base* player) {
	IRangedWeapon::Initialize(weaponID, player);
}

void ShotGun::Update(float deltaTime) {
	rateTimer_ += deltaTime;

	if (rateTimer_ < rate_) {
		return;
	}

	for (int i = 0; i < bulletNum_; ++i) {
		bool falt = false;
		for(int j = 0; j < shots_; ++j) {
			if (!Shot(j == shots_ - 1)) {
				falt = true;
				break;
			}
		}
		if (falt) {
			break;
		}
	}

	ClearIDs();
}
