#pragma once
struct ParameterData {
	float maxHP;               // 最大HP
	float hpRegen;             // HP再生
	float lifeStealPercent;    // ライフスティール％
	float damagePercent;       // 総合ダメージ％
	float meleeDamage;         // 近接ダメージ
	float rangedDamage;        // 遠距離ダメージ
	float attackSpeedPercent;  // 攻撃速度％
	float criticalRatePercent; // クリティカル率％
	float dodgeRatePercent;    // 回避率％
	float defense;             // 防御力
	float moveSpeedPercent;    // 移動速度％
	float expBonusPercent;     // 経験値ボーナス％
	float moneyBonusPercent;   // お金ボーナス％
	float range;               // 攻撃範囲
	float attackCount;         // 攻撃回数
	float penetration;         // 貫通数
	float knockback;           // ノックバック
	float criticalMultiplier;  // クリティカル倍率
};