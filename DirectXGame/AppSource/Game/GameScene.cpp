#include "GameScene.h"

void GameScene::Initialize() {
	gameWindow_ = commonData_->mainWindow.get();
	display_ = commonData_->display.get();
	//カメラの初期化
	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize();
	//パラメーター管理の初期化
	paramManager_ = std::make_unique<ParamManager>();
}

std::unique_ptr<IScene> GameScene::Update() {

	debugCamera_->Update();
	camera_ = *static_cast<Camera*>(debugCamera_.get());

	return nullptr;
}

void GameScene::Draw() {
	display_->PreDraw(gameWindow_->GetCommandList(), true);

	//Playerとかの処理

	display_->PostDraw(gameWindow_->GetCommandList());

	gameWindow_->PreDraw();
	//PostEffectとか
	
	//ImGui
	gameWindow_->DrawDisplayWithImGui();
	paramManager_->Draw();
	engine_->ImGuiDraw();
}
