#include "IAttackObject.h"
#include "../AppSource/GameObject/Enemy/Enemy.h"

void IAttackObject::OnHitTarget(Enemy* target) {
	if (!target || !target->IsAlive()) {
		return;
	}

	// すでに当たっている敵は無視
	if (hitTargets_.find(target->GetID()) != hitTargets_.end()) {
		return;
	}

	// ダメージを与える
	int damage = static_cast<int>(damageInfo_.damage);
	target->TakeDamage(damage);

	// 当たった敵のIDをセットに追加
	hitTargets_.insert(target->GetID());

	// 貫通回数を減らす
	if (penetrationCount_ > 0) {
		penetrationCount_--;
	}
}
