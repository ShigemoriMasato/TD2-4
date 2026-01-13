#include "GameCamera.h"
#include <Utility/Easing.h>
#include <imgui/imgui.h>

GameCamera::~GameCamera() {
	Save();
}

void GameCamera::Initialize() {
	binaryManager_ = std::make_unique<BinaryManager>();
	camera_ = std::make_unique<Camera>();
	camera_->SetProjectionMatrix(PerspectiveFovDesc());
	Load();
}

void GameCamera::Update(float deltaTime) {

	Vector3 shakeOffset{};
	if (shakeTime_ < shakeDuration_) {
		shakeTime_ += deltaTime;
		float shakeAmount = shakeIntensity_ * (1.0f - (shakeTime_ / shakeDuration_));
		shakeOffset = {
			((static_cast<float>(rand()) / RAND_MAX) * 2.0f - 1.0f) * shakeAmount,
			((static_cast<float>(rand()) / RAND_MAX) * 2.0f - 1.0f) * shakeAmount,
			((static_cast<float>(rand()) / RAND_MAX) * 2.0f - 1.0f) * shakeAmount
		};
	}

	factPos_ = lerp(position_ + shakeOffset, beginningPos_, beginningT_, EaseType::EaseInQuint);
	factRot_ = lerp(rotation_, beginningRot_, beginningT_, EaseType::EaseInQuint);

	beginningT_ = std::max(0.0f, beginningT_ - deltaTime);

	camera_->position_ = factPos_;
	camera_->rotation_ = factRot_;

	camera_->MakeMatrix();
}

void GameCamera::Draw() {
}

void GameCamera::Shake(float intensity, float duration) {
	shakeTime_ = 0.0f;
	shakeDuration_ = duration;
	shakeIntensity_ = intensity;
}

void GameCamera::DrawImGui() {
#ifdef USE_IMGUI

	ImGui::Begin("GameCamera");

	ImGui::DragFloat3("Scale", &camera_->scale_.x, 0.1f);
	ImGui::DragFloat3("Rotation", &camera_->rotation_.x, 0.1f);
	ImGui::DragFloat3("Position", &position_.x, 0.1f);

	ImGui::DragFloat3("BeginPosition", &beginningPos_.x, 0.1f);
	ImGui::DragFloat3("BeginRotation", &beginningRot_.x, 0.1f);

	if (ImGui::Button("Lock")) {
		beginningEdit_ = !beginningEdit_;
	}
	if (beginningEdit_) {
		beginningT_ = 1.0f;
	}

	ImGui::End();

#endif
}

void GameCamera::Save() {
	binaryManager_->RegistOutput(position_);
	binaryManager_->RegistOutput(camera_->rotation_);
	binaryManager_->RegistOutput(beginningPos_);
	binaryManager_->RegistOutput(beginningRot_);
	binaryManager_->Write(saveFile_);
}

void GameCamera::Load() {
	auto data = binaryManager_->Read(saveFile_);

	if (data.empty()) {
		return;
	}

	uint32_t index = 0;

	Vector3 position = BinaryManager::Reverse<Vector3>(data[index++].get());
	Vector3 rotate = BinaryManager::Reverse<Vector3>(data[index++].get());

	position_ = position;
	camera_->rotation_ = rotate;

	if (data.size() <= index) {
		return;
	}

	beginningPos_ = BinaryManager::Reverse<Vector3>(data[index++].get());
	beginningRot_ = BinaryManager::Reverse<Vector3>(data[index++].get());
}
