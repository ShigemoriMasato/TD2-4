#include "GameCamera.h"
#include <Utility/Easing.h>
#include <imgui/imgui.h>

void GameCamera::Initialize() {
	//初期位置を設定
	position_ = { 0.0f, 0.0f, -10.0f };
	rotation_.x = -0.5f;
	SetProjectionMatrix(PerspectiveFovDesc());
}

void GameCamera::Update(float deltaTime, Vector3 position) {
	position_ = lerp(position_, position + offset_, deltaTime * 10.0f);
	MakeMatrix();
}

void GameCamera::DrawImGui() {
#ifdef USE_IMGUI
	ImGui::Begin("Camera");

	if (ImGui::Checkbox("Change View", &isViewType2)) {
		if (isViewType2) {
			offset_ = { -6.5f, 27.0f, -40.0f };
			rotation_ = { -0.500f, 0.0f, 0.0f };
		} else {
			offset_ = { -2.0f, 78.0f, 14.0f };
			rotation_ = { -1.570f, 0.0f, 0.0f };
		}
	}

	ImGui::DragFloat3("Offset", &offset_.x, 0.1f);

	ImGui::Separator();

	ImGui::DragFloat3("Scale", &scale_.x, 0.1f);
	ImGui::DragFloat3("Rotation", &rotation_.x, 0.1f);
	ImGui::DragFloat3("Position", &position_.x, 0.1f);

	ImGui::End();
#endif
}
