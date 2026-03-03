#include "IAttackObject.h"

void IAttackObject::OnHitTarget(Enemy* target) {
	//if (hitTargets_.find(target->GetID()) != hitTargets_.end()) {
	//	return; // すでに当たっている敵は無視
	//}

	// ==========================================================
	// ダメージを与える(DamageInfoを渡して計算したいため現在は仮の処理)
	target->TakeDamage(static_cast<int>(damageInfo_.damage));
	// ==========================================================

	//hitTargets_.insert(target->GetID()); // 当たった敵のIDをセットに追加

	// 貫通回数を減らす
	if (penetrationCount_ > 0) {
		penetrationCount_--;
	} else {
		// 貫通回数が0以下になったら攻撃オブジェクトを削除
	}
}
