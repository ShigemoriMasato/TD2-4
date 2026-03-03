#pragma once
#include "../AppSource/GameObject/Enemy/Enemy.h"
#include "../AppSource/GameObject/Weapon/DamageInfo.h"
#include <unordered_set>

class IAttackObject {
public:
	virtual void Update(float deltaTime) = 0;
	virtual void Draw() = 0;

	// 当たり判定処理
	void OnHitTarget(Enemy* target);

protected:
	DamageInfo damageInfo_;              // 攻撃のダメージ情報
	int penetrationCount_ = 0;           // 貫通回数
	std::unordered_set<int> hitTargets_; // すでに当たった敵のIDを保存するセット
};
