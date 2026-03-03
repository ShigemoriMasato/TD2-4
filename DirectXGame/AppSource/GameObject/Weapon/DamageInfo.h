#pragma once
struct DamageInfo {
	float damage;          // 最終ダメージ(baseDamageとStatusManagerの補正)
	float criticalChance;  // クリティカル発生確率
	float criticalMult;    // クリティカル倍率
	float knockbackPower;  // ノックバックの強さ
	float lifeStealChance; // ライフスティール確率
};