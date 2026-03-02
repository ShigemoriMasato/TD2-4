#pragma once
/// <summary>
/// ステータスの種類
/// </summary>
enum class StatusType {
	MaxHP,               // 最大HP
	HPRegen,             // HP再生
	LifeStealPercent,    // ライフスティール％
	DamagePercent,       // 総合ダメージ％
	MeleeDamage,         // 近接ダメージ
	RangedDamage,        // 遠距離ダメージ
	AttackSpeedPercent,  // 攻撃速度％
	CriticalRatePercent, // クリティカル率％
	DodgeRatePercent,    // 回避率％
	Defense,             // 防御力
	MoveSpeedPercent,    // 移動速度％
	ExpBonusPercent,     // 経験値ボーナス％
	MoneyBonusPercent,   // お金ボーナス％
};

/// <summary>
/// 計算のタイプ
/// </summary>
enum class ModifierType {
	Flat,       // 固定値の加算
	PercentAdd, // 割合の加算
	PercentMult // 割合の乗算
};

/// <summary>
/// バフ・デバフのデータ構造
/// </summary>
class StatusModifier {
public:
	// コンストラクタ
	StatusModifier(float value, ModifierType type, const void* source = nullptr) : value_(value), type_(type), source_(source) {}

public:
	float value_;
	ModifierType type_;

	// どのアイテム、スキルによる効果かを記録
	const void* source_;
};
