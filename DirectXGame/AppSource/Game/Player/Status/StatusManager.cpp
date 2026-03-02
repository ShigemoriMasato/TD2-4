#include "StatusManager.h"

StatusManager::StatusManager() {
	// 初期ステータスのセットアップ
	statuses_[StatusType::MaxHP] = Status(50.0f);              // 最大HP
	statuses_[StatusType::HPRegen] = Status(0.0f);             // HP再生
	statuses_[StatusType::LifeStealPercent] = Status(0.0f);    // ライフスティール％
	statuses_[StatusType::DamagePercent] = Status(0.0f);       // 総合ダメージ％
	statuses_[StatusType::MeleeDamage] = Status(0.0f);         // 近接ダメージ
	statuses_[StatusType::RangedDamage] = Status(0.0f);        // 遠距離ダメージ
	statuses_[StatusType::AttackSpeedPercent] = Status(0.0f);  // 攻撃速度％
	statuses_[StatusType::CriticalRatePercent] = Status(0.0f); // クリティカル率％
	statuses_[StatusType::DodgeRatePercent] = Status(0.0f);    // 回避率％
	statuses_[StatusType::Defense] = Status(0.0f);             // 防御力
	statuses_[StatusType::MoveSpeedPercent] = Status(0.0f);    // 移動速度％
	statuses_[StatusType::ExpBonusPercent] = Status(0.0f);     // 経験値ボーナス％
	statuses_[StatusType::MoneyBonusPercent] = Status(0.0f);   // お金ボーナス％
}

void StatusManager::AddModifier(StatusType type, const StatusModifier& modifier) {
	// モディファイアーの追加
	statuses_[type].AddModifier(modifier);
}

void StatusManager::RemoveModifierFromItem(const void* itemSource) {
	for (auto& pair : statuses_) {
		pair.second.RemoveModifierFromSource(itemSource);
	}
}
