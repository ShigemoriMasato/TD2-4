#include "WeaponDebugger.h"
#include "WeaponManager.h"
#include <imgui/imgui.h>

void WeaponDebugger::Draw() {
	if (!weaponManager_)
		return;

#ifdef USE_IMGUI
	ImGui::Begin("Weapon Debugger");
	ImGui::InputInt("Target Weapon ID", &currentWeaponId);
	ImGui::Separator();

	// IDに基づいて武器データを取得
	WeaponData* weapon = weaponManager_->GetWeapon(currentWeaponId);

	// 武器データが見つかった場合
	if (weapon != nullptr) {
		//武器の種類
		std::vector<const char*> weaponTypeNames;
		for (const auto& pair : weaponTypeNames_) {
			weaponTypeNames.push_back(pair.second.c_str());
		}
		int weaponTypeIndex = static_cast<int>(weapon->type);
		if (ImGui::Combo("Weapon Type", &weaponTypeIndex, weaponTypeNames.data(), (uint32_t)weaponTypeNames.size())) {
			weapon->type = static_cast<WeaponType>(weaponTypeIndex);
		}

		// レア度表示
		const char* rarityNames[] = {"Common", "Uncommon", "Rare", "Epic"};
		int rarityIndex = weapon->rarity;
		if (ImGui::Combo("Rarity", &rarityIndex, rarityNames, IM_ARRAYSIZE(rarityNames))) {
			weapon->rarity = rarityIndex;
		}

		ImGui::Separator();

		// 数値の調整
		ImGui::SliderFloat("基本の攻撃力", &weapon->baseDamage, 1.0f, 999.0f, "%.1f");
		ImGui::SliderFloat("攻撃速度", &weapon->attackSpeed, 0.1f, 5.0f, "%.2f sec");
		ImGui::DragFloat("偏差", &weapon->spreadAngle, 0.01f, 0.0f, 6.28f, "%.1f deg");
		ImGui::SliderFloat("クリティカル発生確率", &weapon->criticalChance, 0.0f, 80.0f, "%.2f");
		ImGui::SliderFloat("クリティカルダメージ倍率", &weapon->criticalMultiplier, 0.1f, 2.0f, "%.2f");
		ImGui::SliderFloat("ライフスティール確率", &weapon->lifeStealChance, 0.0f, 80.0f, "%.2f");
		ImGui::SliderFloat("ノックバック距離", &weapon->knockbackPower, 0.0f, 100.0f, "%.1f");
		ImGui::SliderFloat("射程距離", &weapon->range, 0.1f, 100.0f, "%.1f");
		ImGui::SliderInt("貫通回数", &weapon->penetration, 0, 10, "%d");
		ImGui::SliderInt("バックパック内でのサイズ", &weapon->size, 1, 9);
		ImGui::SliderInt("攻撃回数", &weapon->attackCount, 1, 9);

		ImGui::Separator();

		// 保存ボタン
		if (ImGui::Button("Save Weapon Data", ImVec2(-1, 0))) {
			weaponManager_->SaveWeaponData();
			ImGui::OpenPopup("SaveSuccess");
		}
	} else {
		// 武器データが見つからない場合
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Weapon ID %d not found", currentWeaponId);
	}

	// 読み込みボタン
	ImGui::Separator();
	if (ImGui::Button("Load Weapon Data", ImVec2(-1, 0))) {
		weaponManager_->LoadWeaponData();
		ImGui::OpenPopup("LoadSuccess");
	}

	ImGui::End();
#endif
}