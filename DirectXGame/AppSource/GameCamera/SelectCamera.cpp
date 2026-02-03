#include"SelectCamera.h"
#include"FpsCount.h"
#include"DebugMousePos.h"
#include"Utility/Easing.h"

void SelectCamera::Initialize() {

	// カメラを初期化
	this->SetProjectionMatrix(PerspectiveFovDesc{});
	position_ = { 0.0f,3.9f,-14.0f };
	rotation_ = { -0.32f,0.0f,0.0f };
	MakeMatrix();
}

void SelectCamera::Update() {

	// 追従する位置
	FollowPos();

	// カメラの更新処理
	MakeMatrix();

	// ワールド座標の位置を取得する
#ifdef USE_IMGUI
	DebugMousePos::gameMousePos;
#else
	DebugMousePos::screenMousePos, 1280.0f, 720.0f;
#endif
}

void SelectCamera::DebugDraw() {
	// デバック描画
	DrawImGui();
}

void SelectCamera::FollowPos() {
	// 追従対象とオフセットと追従対象の速度からカメラの目標座標を計算
	TargetCoordinate_ = targetPos_ + targetVelocity_ * (kVelocityBias * FpsCount::deltaTime);

	// 座標補間によりゆったり追従
	position_.x = lerp(position_.x, TargetCoordinate_.x, kInterpolationRate,EaseType::Linear);
}

void SelectCamera::MakeMatrix() {
	transformMatrix_ = Matrix::MakeTranslationMatrix(-position_) * Matrix::MakeRotationMatrix(rotation_);
	worldMatrix_ = Matrix::MakeAffineMatrix(scale_, rotation_, position_);
	vpMatrix_ = Matrix::MakeTranslationMatrix(-position_) * Matrix::MakeRotationMatrix(rotation_) * projectionMatrix_;
}
