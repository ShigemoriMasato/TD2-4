#pragma once
#include "IRangedWeapon.h"

class ShotGun : public IRangedWeapon {
public:

private:

	void Shot(IEnemy* target) override;

	const int shots_ = 5; //一回に飛ばすショットガンの弾数

};
