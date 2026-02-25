#include "PlayerLevelSystem.h"

void PlayerLevelSystem::Initialize(int startLevel) {
	// レベルの初期化
	level_ = startLevel;

	// 現在の経験値の初期化
	currentExp_ = 0.0f;

	// レベルアップに必要な経験値を更新
	UpdateNextExp();
}

void PlayerLevelSystem::AddExp(float amount) {
	if (amount <= 0)
		return;

	// 経験値加算
	currentExp_ += amount;

	// UI更新イベント
	if (onExpChanged) {
		onExpChanged(currentExp_, nextExp_);
	}

	// レベルアップ判定
	while (currentExp_ >= nextExp_) {
		currentExp_ -= nextExp_;
		level_++;

		// レベルアップに必要な経験値を更新
		UpdateNextExp();

		if (onLevelUp) {
			onLevelUp(level_);
		}
	}
}

void PlayerLevelSystem::UpdateNextExp() {
	// レベルアップに必要な経験値を更新
	nextExp_ = 10.0f + level_ * 5.0f;
}

void PlayerLevelSystem::DrawImGui() {
#ifdef _DEBUG
	// ImGuiの描画
	ImGui::Begin("PlayerLevelSyastem");
	ImGui::Text("PlayerLevel : %d", level_);
	ImGui::Text("CurrentEXP : %.2f", currentExp_);
	ImGui::Text("NextEXP : %.2f", nextExp_);
	ImGui::End();
#endif
}
