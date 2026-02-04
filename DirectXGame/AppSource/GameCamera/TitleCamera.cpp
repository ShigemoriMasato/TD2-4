#include"TitleCamera.h"
#include"FpsCount.h"
#include"Utility/Easing.h"

void TitleCamera::Initialize() {

	// カメラを初期化
	this->SetProjectionMatrix(PerspectiveFovDesc{});
	position_ = { -22.5f,4.2f,-22.4f };
	rotation_ = { 0.0f,-0.8f,0.0f };
	MakeMatrix();
}

void TitleCamera::Update() {

	timer_ += FpsCount::deltaTime / maxTime_;

	// 移動
	position_ = lerp(Vector3(-22.5f, 4.2f, -22.4f), Vector3(0.0f, 6.1f, -16.0f), timer_, EaseType::EaseInOutCubic);
	// 回転
	rotation_.x = lerp(0.0f, -0.32f, timer_, EaseType::EaseInOutCubic);
	rotation_.y = lerp(-0.8f, 0.0f, timer_, EaseType::EaseInOutCubic);

	if (timer_ >= 1.0f) {
		isAnimation_ = false;
	}

	// カメラの更新処理
	MakeMatrix();
}

void TitleCamera::DebugDraw() {
	// デバック描画
	DrawImGui();
}

void TitleCamera::MakeMatrix() {
	transformMatrix_ = Matrix::MakeTranslationMatrix(-position_) * Matrix::MakeRotationMatrix(rotation_);
	worldMatrix_ = Matrix::MakeAffineMatrix(scale_, rotation_, position_);
	vpMatrix_ = Matrix::MakeTranslationMatrix(-position_) * Matrix::MakeRotationMatrix(rotation_) * projectionMatrix_;
}
