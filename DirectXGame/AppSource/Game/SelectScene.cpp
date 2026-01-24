#include"SelectScene.h"
#include <Common/DebugParam/GameParamEditor.h>
#include"FpsCount.h"
#include"GameCamera/DebugMousePos.h"

namespace {
	//Minimap確認用
	bool poseMode = false;

}

void SelectScene::Initialize() {
	gameWindow_ = commonData_->mainWindow.get();
	display_ = commonData_->display.get();
	//カメラの初期化
	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize();
	//パラメーター管理の初期化
	paramManager_ = std::make_unique<ParamManager>();

	postEffect_ = std::make_unique<PostEffect>();
	postEffect_->Initialize(textureManager_, drawDataManager_->GetDrawData(commonData_->postEffectDrawDataIndex));

	postEffectConfig_.window = gameWindow_->GetWindow();
	postEffectConfig_.origin = display_;
	postEffectConfig_.jobs_ = 0;
}

std::unique_ptr<IScene> SelectScene::Update() {

	// Δタイムを取得する
	FpsCount::deltaTime = engine_->GetFPSObserver()->GetDeltatime();

	// 入力処理の更新
	input_->Update();
	commonData_->keyManager->Update();

	debugCamera_->Update();
	camera_ = *static_cast<Camera*>(debugCamera_.get());

	return nullptr;
}

void SelectScene::Draw() {
	display_->PreDraw(gameWindow_->GetCommandObject(), true);

	display_->PostDraw(gameWindow_->GetCommandObject());

	//PostEffectとか
	postEffectConfig_.output = gameWindow_->GetDualDisplay();
	postEffect_->Draw(postEffectConfig_);

	gameWindow_->PreDraw();

	//ImGui
	gameWindow_->DrawDisplayWithImGui();
	paramManager_->Draw();

	engine_->ImGuiDraw();
}
