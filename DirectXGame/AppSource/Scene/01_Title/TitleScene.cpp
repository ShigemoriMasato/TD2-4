#include "TitleScene.h"
#include <imgui/imgui.h>
#include <Utility/Color.h>
#include <Utility/MatrixFactory.h>

#include "04_Asakawa/AsakawaScene.h"

TitleScene::TitleScene() {
}

void TitleScene::Initialize() {
	// TitleUIの生成と初期化
	titleUI_ = std::make_unique<TitleUI>();
	titleUI_->Initialize(modelManager_, drawDataManager_);

	// カメラの生成&初期化
	camera_ = std::make_unique<Camera>();

	// カメラの投影行列を設定（透視投影）
	PerspectiveFovDesc perspectiveDesc{};
	perspectiveDesc.fovY = 0.45f;
	perspectiveDesc.aspectRatio = 16.0f / 9.0f;
	perspectiveDesc.nearClip = 0.1f;
	perspectiveDesc.farClip = 1000.0f;
	camera_->SetProjectionMatrix(perspectiveDesc);

	// カメラのトランスフォーム設定
	cameraTransform_.position = { 0.0f, 0.0f, 0.0f };
	cameraTransform_.rotate = { 0.0f, 0.0f, 0.0f };
	cameraTransform_.scale = { 1.0f, 1.0f, 1.0f };
}

std::unique_ptr<IScene> TitleScene::Update() {
	// カメラのトランスフォーム更新
	Matrix4x4 cameraMatrix = Matrix::MakeAffineMatrix(cameraTransform_.scale, cameraTransform_.rotate, cameraTransform_.position);
	camera_->SetTransform(cameraMatrix);
	camera_->MakeMatrix();
	
	// TitleUIの更新
	titleUI_->Update(camera_->GetVPMatrix());
	
	// ZキーでAsakawaSceneに遷移
	if (input_->GetKeyState(DIK_Z) && !input_->GetPreKeyState(DIK_Z)) {
		return std::make_unique<AsakawaScene>();
	}
	
	return nullptr;
}

void TitleScene::Draw() {
	auto window = commonData_->mainWindow.second.get();
	auto display = commonData_->display.get();
	auto cmdObj = commonData_->cmdObject.get();

	display->PreDraw(cmdObj, true);
	
	// TitleUIの描画
	titleUI_->Draw(cmdObj);
	
	display->PostDraw(cmdObj);

	window->PreDraw(cmdObj);

	//ここ以外で記述する場合、ifdefを忘れないようにすること
#ifdef USE_IMGUI

	display->DrawImGui();

	ImGui::Begin("FPS");
	float deltaTime = engine_->GetFPSObserver()->GetDeltatime();
	ImGui::Text("DeltaTime: %.3f ms", deltaTime * 1000.0f);
	ImGui::Text("FPS: %.1f", 1.0f / deltaTime);
	ImGui::End();

	// カメラのデバッグ用ImGui
	ImGui::Begin("TitleCamera");
	ImGui::DragFloat3("Scale", &cameraTransform_.scale.x, 0.01f);
	ImGui::DragFloat3("Rotate", &cameraTransform_.rotate.x, 0.01f);
	ImGui::DragFloat3("Position", &cameraTransform_.position.x, 0.01f);
	ImGui::End();

	// TitleUIのデバッグ用ImGui
	titleUI_->DrawImGui();
#endif

	engine_->DrawImGui();
	window->PostDraw(cmdObj);
}
