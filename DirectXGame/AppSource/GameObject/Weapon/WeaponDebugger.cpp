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
		ImGui::SliderFloat("クリティカル発生確率", &weapon->criticalChance, 0.0f, 80.0f, "%.2f");
		ImGui::SliderFloat("クリティカルダメージ倍率", &weapon->criticalMultiplier, 0.1f, 2.0f, "%.2f");
		ImGui::SliderFloat("ライフスティール確率", &weapon->lifeStealChance, 0.0f, 80.0f, "%.2f");
		ImGui::SliderFloat("ノックバック距離", &weapon->knockbackPower, 0.0f, 100.0f, "%.1f");
		ImGui::SliderFloat("射程距離", &weapon->range, 0.1f, 100.0f, "%.1f");
		ImGui::SliderInt("貫通回数", &weapon->penetration, 1, 10, "%d");
		ImGui::SliderInt("バックパック内でのサイズ", &weapon->size, 1, 9);

		struct WeaponData {
			int id;                   // ID
			int rarity;               // レア度（WeaponRarityのint値）
			float gold;               // 値段
			float baseDamage;         // 基本ダメージ数
			float criticalChance;     // クリティカル発生確率
			float criticalMultiplier; // クリティカルダメージ倍率
			float lifeStealChance;    // ライフスティール確率
			float attackSpeed;        // 攻撃速度
			float knockbackPower;     // ノックバックの強さ
			float range;              // 射程距離
			int penetration;          // 貫通
			float spreadAngle;        // 拡散角度
			int size;                 // バックパック上でのサイズ
			char nameBuffer[64];      // ImGuiのInputText用の固定長のバッファ
		};

		ImGui::Separator();

		// 保存ボタン
		if (ImGui::Button("Save Weapon Data", ImVec2(-1, 0))) {
			weaponManager_->SaveWeaponData();
			ImGui::OpenPopup("SaveSuccess");
		}

		// 保存成功ダイアログ
		if (ImGui::BeginPopupModal("SaveSuccess", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
			ImGui::Text("Weapon data saved successfully!");
			if (ImGui::Button("OK", ImVec2(120, 0))) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
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

	// 読み込み成功ダイアログ
	if (ImGui::BeginPopupModal("LoadSuccess", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("Weapon data loaded successfully!");
		if (ImGui::Button("OK", ImVec2(120, 0))) {
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	ImGui::End();
#endif
}