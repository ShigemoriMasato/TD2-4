#include "GameCamera.h"
#include <Utility/Easing.h>
#include <imgui/imgui.h>
#include <Input/Input.h>

void GameCamera::Initialize() {
	//初期位置を設定
	position_ = { 0.0f, 0.0f, -10.0f };
	rotation_.x = -0.5f;
	SetProjectionMatrix(PerspectiveFovDesc());
}

void GameCamera::Update(float deltaTime, Vector3 position) {
	// マウスホイールによるカメラ距離の調整
	if (input_) {
		float mouseWheel = input_->GetMouseWheel();
		if (mouseWheel != 0.0f) {
			// ホイール回転量に応じて距離スケールを調整
			distanceScale_ += mouseWheel * wheelSensitivity_;
			distanceScale_ = std::clamp(distanceScale_, minDistanceScale_, maxDistanceScale_);
		}
	}
	
	// offsetに距離スケールを適用
	Vector3 scaledOffset = offset_ * distanceScale_;
	position_ = lerp(position_, position + scaledOffset, deltaTime * 10.0f);
	MakeMatrix();
}

void GameCamera::DrawImGui() {
#ifdef USE_IMGUI
	ImGui::Begin("Camera");

	if (ImGui::Checkbox("Change View", &isViewType2)) {
		if (isViewType2) {
			offset_ = { 0.0f, 27.0f, -40.0f };
			rotation_ = { -0.500f, 0.0f, 0.0f };
		} else {
			offset_ = { -2.0f, 78.0f, 14.0f };
			rotation_ = { -1.570f, 0.0f, 0.0f };
		}
	}

	ImGui::DragFloat3("Offset", &offset_.x, 0.1f);
	ImGui::SliderFloat("Distance Scale", &distanceScale_, minDistanceScale_, maxDistanceScale_);

	ImGui::Separator();

	ImGui::DragFloat3("Scale", &scale_.x, 0.1f);
	ImGui::DragFloat3("Rotation", &rotation_.x, 0.1f);
	ImGui::DragFloat3("Position", &position_.x, 0.1f);

	ImGui::End();
#endif
}
