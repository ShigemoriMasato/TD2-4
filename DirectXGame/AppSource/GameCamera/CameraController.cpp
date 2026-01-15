#include"CameraController.h"
#include<numbers>

void CameraController::Initialize(Input* input) {

	// 入力処理を取得
	input_ = input;

	// カメラを初期化
	camera_ = std::make_unique<Camera>();
	camera_->SetProjectionMatrix(PerspectiveFovDesc{});
	camera_->position_ = { 0.0f,10.0f,0.0f };
	camera_->rotation_ = { std::numbers::pi_v<float> / 2.0f,0.0f,0.0f };
	camera_->MakeMatrix();
}

void CameraController::Update() {

	// マウス座標を取得
	//Vector2 mouseMove = input_->GetMouseMove();

	// 0:左, 1:右, 2:中
	BYTE* mouseKey = input_->GetMouseButtonState();

	// 左クリックを押している状態
	if (mouseKey && (mouseKey[0] & 0x80)) {

	}



	// カメラの更新処理
	camera_->MakeMatrix();
}

void CameraController::DebugDraw() {
	// デバック描画
	camera_->DrawImGui();
}