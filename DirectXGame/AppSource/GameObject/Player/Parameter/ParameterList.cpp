#include "ParameterList.h"

void ParameterList::Initialize(JsonManager* jsonManager) {
	// JsonManagerの参照を保存
	jsonManager_ = jsonManager;

	// キャラクターIDの最大値に合わせてリサイズ
	parameterData_.resize(static_cast<int>(CharacterID::Size));

	for (int i = 0; i < parameterData_.size() - 1; ++i) {
		jsonManager_->Boot("ParameterData" + std::to_string(i));

		// JsonManagerからパラメータデータを取得してリストに保存
		parameterData_[i].maxHP = jsonManager_->Get<float>("MaxHP");
		parameterData_[i].hpRegen = jsonManager_->Get<float>("HPRegen");
		parameterData_[i].lifeStealPercent = jsonManager_->Get<float>("LifeStealPercent");
		parameterData_[i].damagePercent = jsonManager_->Get<float>("DamagePercent");
		parameterData_[i].meleeDamage = jsonManager_->Get<float>("MeleeDamage");
		parameterData_[i].rangedDamage = jsonManager_->Get<float>("RangedDamage");
		parameterData_[i].attackSpeedPercent = jsonManager_->Get<float>("AttackSpeedPercent");
		parameterData_[i].criticalRatePercent = jsonManager_->Get<float>("CriticalRatePercent");
		parameterData_[i].dodgeRatePercent = jsonManager_->Get<float>("DodgeRatePercent");
		parameterData_[i].defense = jsonManager_->Get<float>("Defense");
		parameterData_[i].moveSpeedPercent = jsonManager_->Get<float>("MoveSpeedPercent");
		parameterData_[i].expBonusPercent = jsonManager_->Get<float>("ExpBonusPercent");
		parameterData_[i].moneyBonusPercent = jsonManager_->Get<float>("MoneyBonusPercent");
		parameterData_[i].range = jsonManager_->Get<float>("Range");
	}
}