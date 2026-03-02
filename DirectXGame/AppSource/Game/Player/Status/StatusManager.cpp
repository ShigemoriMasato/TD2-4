#include "StatusManager.h"

StatusManager::StatusManager() {
	jsonManager_ = std::make_unique<JsonManager>();
	jsonManager_->Boot("PlayerStatus");

	// 初期ステータスのセットアップ
	statuses_[StatusType::MaxHP] = Status(jsonManager_->Get<float>("MaxHP"));                             // 最大HP
	statuses_[StatusType::HPRegen] = Status(jsonManager_->Get<float>("HPRegen"));                         // HP再生
	statuses_[StatusType::LifeStealPercent] = Status(jsonManager_->Get<float>("LifeStealPercent"));       // ライフスティール％
	statuses_[StatusType::DamagePercent] = Status(jsonManager_->Get<float>("DamagePercent"));             // 総合ダメージ％
	statuses_[StatusType::MeleeDamage] = Status(jsonManager_->Get<float>("MeleeDamage"));                 // 近接ダメージ
	statuses_[StatusType::RangedDamage] = Status(jsonManager_->Get<float>("RangedDamage"));               // 遠距離ダメージ
	statuses_[StatusType::AttackSpeedPercent] = Status(jsonManager_->Get<float>("AttackSpeedPercent"));   // 攻撃速度％
	statuses_[StatusType::CriticalRatePercent] = Status(jsonManager_->Get<float>("CriticalRatePercent")); // クリティカル率％
	statuses_[StatusType::DodgeRatePercent] = Status(jsonManager_->Get<float>("DodgeRatePercent"));       // 回避率％
	statuses_[StatusType::Defense] = Status(jsonManager_->Get<float>("Defense"));                         // 防御力
	statuses_[StatusType::MoveSpeedPercent] = Status(jsonManager_->Get<float>("MoveSpeedPercent"));       // 移動速度％
	statuses_[StatusType::ExpBonusPercent] = Status(jsonManager_->Get<float>("ExpBonusPercent"));         // 経験値ボーナス％
	statuses_[StatusType::MoneyBonusPercent] = Status(jsonManager_->Get<float>("MoneyBonusPercent"));     // お金ボーナス％
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
