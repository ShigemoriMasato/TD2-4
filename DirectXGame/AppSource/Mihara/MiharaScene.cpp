#include "MiharaScene.h"
#include <Utility/Color.h>
#include <Utility/MatrixFactory.h>
#include <imgui/imgui.h>

void MiharaScene::Initialize() {
	// プレイヤーの生成&初期化
	player_ = std::make_unique<Player>();
	player_->Initialize(modelManager_, drawDataManager_, input_);

	// プレイヤーHPの生成&初期化
	playerHP_ = std::make_unique<PlayerHP>();
	playerHP_->Initialize(modelManager_, drawDataManager_, input_);

	// デバッグカメラの生成&初期化
	camera_ = std::make_unique<Camera>();

	// カメラの投影行列を設定
	PerspectiveFovDesc perspectiveDesc{};
	perspectiveDesc.fovY = 0.45f;
	perspectiveDesc.aspectRatio = 16.0f / 9.0f;
	perspectiveDesc.nearClip = 0.1f;
	perspectiveDesc.farClip = 1000.0f;
	camera_->SetProjectionMatrix(perspectiveDesc);

	// 武器のパラメータ管理インスタンス生成&初期化
	weaponManager_ = std::make_unique<WeaponManager>();
	weaponManager_->InitializeData();

	// 武器のパラメータ管理デバッガーの生成&初期化
	weaponDebugger_ = std::make_unique<WeaponDebugger>(weaponManager_.get());

	// カメラのトランスフォーム設定
	cameraTransform_.position = {0.0f, -2.5f, 35.0f};
	cameraTransform_.rotate = {-0.3f, 0.0f, 0.0f};
	cameraTransform_.scale = {1.0f, 1.0f, 1.0f};
}

std::unique_ptr<IScene> MiharaScene::Update() {
	Matrix4x4 cameraMatrix = Matrix::MakeAffineMatrix(cameraTransform_.scale, cameraTransform_.rotate, cameraTransform_.position);
	camera_->SetTransform(cameraMatrix);
	camera_->MakeMatrix();

	// プレイヤーの更新
	player_->Update(camera_->GetVPMatrix());

	// プレイヤーのHP更新
	playerHP_->Update(camera_->GetVPMatrix());

	return nullptr;
}

void MiharaScene::Draw() {
	auto window = commonData_->mainWindow.second.get();
	auto display = commonData_->display.get();
	auto cmdObj = commonData_->cmdObject.get();

	display->PreDraw(cmdObj, true);

	// プレイヤーの描画
	player_->Draw(cmdObj);

	// プレイヤーHPの描画
	playerHP_->Draw(cmdObj);

	display->PostDraw(cmdObj);

	window->PreDraw(cmdObj);

	// ここ以外で記述する場合、ifdefを忘れないようにすること
#ifdef USE_IMGUI

	display->DrawImGui();

	ImGui::Begin("FPS");
	float deltaTime = engine_->GetFPSObserver()->GetDeltatime();
	ImGui::Text("DeltaTime: %.3f ms", deltaTime * 1000.0f);
	ImGui::Text("FPS: %.1f", 1.0f / deltaTime);
	ImGui::End();

	ImGui::Begin("Mihara Scene");
	ImGui::Text("This is Mihara Scene!!!!!!");
	ImGui::End();

	ImGui::Begin("Camera");
	ImGui::DragFloat3("Scale", &cameraTransform_.scale.x, 0.01f);
	ImGui::DragFloat3("Rotate", &cameraTransform_.rotate.x, 0.01f);
	ImGui::DragFloat3("Position", &cameraTransform_.position.x, 0.01f);
	ImGui::End();

	// 武器のパラメータ管理デバッグ用描画
	weaponDebugger_->Draw();
#endif

	engine_->DrawImGui();
	window->PostDraw(cmdObj);
}
