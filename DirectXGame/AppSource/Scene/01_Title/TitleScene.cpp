#include "TitleScene.h"
#include <imgui/imgui.h>
#include <Utility/Color.h>

TitleScene::TitleScene() {
}

void TitleScene::Initialize() {
	titleUI_ = std::make_unique<TitleUI>();
	titleUI_->Initialize(drawDataManager_, modelManager_);
	
	camera_ = std::make_unique<Camera>();
	PerspectiveFovDesc perspectiveDesc;
	perspectiveDesc.SetValue(1280, 720, 0.45f, 0.1f, 1000.0f);
	camera_->SetProjectionMatrix(perspectiveDesc);
	camera_->SetPosition({ 0.0f, 0.0f, -10.0f });
	camera_->SetRotation({ 0.0f, 0.0f, 0.0f });
	camera_->SetScale({ 1.0f, 1.0f, 1.0f });
}

std::unique_ptr<IScene> TitleScene::Update() {
	// ZキーでCharaSelectSceneに遷移
	if (input_->GetKeyState(DIK_Z) && !input_->GetPreKeyState(DIK_Z)) {
		//return std::make_unique<AsakawaScene>();
	}
	
	// カメラの行列更新
	camera_->MakeMatrix();
	
	// VP行列を取得してUIを更新
	Matrix4x4 vpMatrix = camera_->GetVPMatrix();
	titleUI_->Update(vpMatrix);
	
	return nullptr;
}

void TitleScene::Draw() {
	auto window = commonData_->mainWindow.second.get();
	auto display = commonData_->display.get();
	auto cmdObj = commonData_->cmdObject.get();

	// ディスプレイへの描画開始
	display->PreDraw(cmdObj, true);
	
	// TitleUIの描画（displayに描画）
	titleUI_->Draw(cmdObj);
	
	// ディスプレイへの描画終了
	display->PostDraw(cmdObj);

	// ウィンドウへの描画（displayの内容を転送）
	window->PreDraw(cmdObj);

	//ここ以外で記述する場合、ifdefを忘れないようにすること
#ifdef USE_IMGUI

	ImGui::Begin("Title Scene Settings");
	
	// カメラの設定
	if (ImGui::TreeNode("Camera")) {
		static Vector3 cameraPos = { 0.0f, 0.0f, -10.0f };
		static Vector3 cameraRot = { 0.0f, 0.0f, 0.0f };
		static Vector3 cameraScale = { 1.0f, 1.0f, 1.0f };
		
		if (ImGui::DragFloat3("Position", &cameraPos.x, 0.1f)) {
			camera_->SetPosition(cameraPos);
		}
		
		if (ImGui::DragFloat3("Rotation", &cameraRot.x, 0.01f)) {
			camera_->SetRotation(cameraRot);
		}
		
		if (ImGui::DragFloat3("Scale", &cameraScale.x, 0.01f, 0.01f, 10.0f)) {
			camera_->SetScale(cameraScale);
		}
		
		ImGui::TreePop();
	}
	
	ImGui::End();
	
	// TitleUIの設定
	titleUI_->DrawImGui();

	display->DrawImGui();

	ImGui::Begin("FPS");
	float deltaTime = engine_->GetFPSObserver()->GetDeltatime();
	ImGui::Text("DeltaTime: %.3f ms", deltaTime * 1000.0f);
	ImGui::Text("FPS: %.1f", 1.0f / deltaTime);
	ImGui::End();
#endif

	engine_->DrawImGui();
	
	// ウィンドウへの描画終了
	window->PostDraw(cmdObj);
}
