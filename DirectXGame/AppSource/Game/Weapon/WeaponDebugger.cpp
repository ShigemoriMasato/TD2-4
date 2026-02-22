#include "WeaponDebugger.h"
#include "WeaponManager.h"
#include <imgui/imgui.h>

void WeaponDebugger::Draw() {
	if (!weaponManager_)
		return;

	ImGui::Begin("Weapon Debugger");
	ImGui::InputInt("Target Weapon ID", &currentWeaponId); // 編集したい武器のIDを入力
	ImGui::Separator();

	// IDに基づいて武器データを取得
	WeaponData* weapon = weaponManager_->GetWeapon(currentWeaponId);

	// 武器データが見つかった場合
	if (weapon != nullptr) {
		// 編集UIを表示
		ImGui::Text("Editing: %s (ID: %d)", weapon->name.c_str(), weapon->id);

		// 名前の編集
		if (ImGui::InputText("Name", weapon->nameBuffer, IM_ARRAYSIZE(weapon->nameBuffer))) {
			weapon->name = weapon->nameBuffer; // 変更があれば反映
		}

		// 数値の調整
		ImGui::SliderFloat("Base Damage", &weapon->baseDamage, 1.0f, 999.0f, "%.1f");
		ImGui::SliderFloat("Attack Speed", &weapon->attackSpeed, 0.1f, 5.0f, "%.2f sec");
		ImGui::SliderFloat("Price", &weapon->price, 0.0f, 500.0f, "%f gold");
		ImGui::SliderInt("Size", &weapon->size, 1, 9);
		ImGui::SliderInt("Tier", &weapon->tier, 1, 4);

		ImGui::Separator();
	} else {
		// 武器データが見つからない場合
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Weapon ID %d not found", currentWeaponId);
	}

	ImGui::End();
}