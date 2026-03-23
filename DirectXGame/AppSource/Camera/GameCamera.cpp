#include "GameCamera.h"
#include <Utility/Easing.h>
#include <imgui/imgui.h>

void GameCamera::Initialize() {
	//初期位置を設定
	position_ = { -1.5f, 53.0f, 10.5f };
	rotation_ = { -1.57f, 0.0f, 0.0f };
	SetProjectionMatrix(PerspectiveFovDesc());
}

void GameCamera::Update(float deltaTime, Vector3 position) {
	position_ = lerp(position_, position + offset_, deltaTime * 10.0f);
	MakeMatrix();
}

void GameCamera::DrawImGui() {
#ifdef USE_IMGUI
	ImGui::Begin("Camera");

	if (ImGui::Checkbox("Change View", &viewToggle_)) {
		if (viewToggle_) {
			offset_ = { -1.0f, 30.0f, -40.1f };
			rotation_ = { -0.57f, 0.0f, 0.0f };
		}
		else {
			offset_ = { -1.5f, 53.0f, 10.5f };
			rotation_ = { -1.57f, 0.0f, 0.0f };
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
