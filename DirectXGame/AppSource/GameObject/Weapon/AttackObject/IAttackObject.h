#pragma once
#include "../AppSource/GameObject/Weapon/DamageInfo.h"
#include <unordered_set>
#include <Core/Command/CommandManager.h>

class Enemy;

/// <summary>
/// 武器から生成される斬撃や弾のオブジェクト
/// </summary>
class IAttackObject {
public:
	virtual ~IAttackObject() = default;

	virtual void Update(float deltaTime) = 0;
	virtual void Draw(CmdObj* cmdObj) = 0;

	// 当たり判定処理
	void OnHitTarget(Enemy* target);

	// 攻撃が有効かどうか
	virtual bool IsActive() const = 0;

protected:
	DamageInfo damageInfo_;              // 攻撃のダメージ情報
	int penetrationCount_ = 0;           // 貫通回数
	std::unordered_set<int> hitTargets_; // すでに当たった敵のIDを保存するセット
};
