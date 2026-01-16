#include "GameScene.h"
#include <Common/DebugParam/GameParamEditor.h>

namespace {
	//Minimap確認用
	bool poseMode = false;

	std::string playerModelName = "Player";
}

void GameScene::Initialize() {
	gameWindow_ = commonData_->mainWindow.get();
	display_ = commonData_->display.get();
	//カメラの初期化
	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize();
	//パラメーター管理の初期化
	paramManager_ = std::make_unique<ParamManager>();

	auto inst = GameParamEditor::GetInstance();
	inst->SetActiveScene("GameScene");
	inst->CreateGroup("Test", "GameScene");
	inst->AddItem("Test", "param1", param1, 0);
	inst->AddItem("Test", "param2", param2, 1);
	inst->AddItem("Test", "param3", param3, 2);
	inst->AddItem("Test", "param4", param4, 3);
	inst->AddItem("Test", "param5", param5, 4);

	// ゲームカメラを設定
	cameraController_ = std::make_unique<CameraController>();
	cameraController_->Initialize(input_);

	// マップデータ解釈機能を初期化
	mapChipField_ = std::make_unique<MapChipField>();
	// デバックで使用するマップデータを構築
	mapChipField_->SetDebugMapData();

	// プレイヤーモデルを取得
	int playerModelID = modelManager_->LoadModel(playerModelName);
	auto playerModel = modelManager_->GetNodeModelData(playerModelID);

	// おれモデルを取得
	int oreModelID = modelManager_->LoadModel(playerModelName);
	auto oreModel = modelManager_->GetNodeModelData(oreModelID);

	// ユニットの管理クラス
	unitManager_ = std::make_unique<UnitManager>();
	unitManager_->Initalize(mapChipField_.get(),
		drawDataManager_->GetDrawData(playerModel.drawDataIndex), drawDataManager_->GetDrawData(oreModel.drawDataIndex),
		commonData_->keyManager.get());

	postEffect_ = std::make_unique<PostEffect>();
	postEffect_->Initialize(textureManager_, drawDataManager_->GetDrawData(commonData_->postEffectDrawDataIndex));

	postEffectConfig_.window = gameWindow_->GetWindow();
	postEffectConfig_.origin = display_;
	postEffectConfig_.jobs_ = (uint32_t) PostEffectJob::Blur | PostEffectJob::Fade | PostEffectJob::Glitch;
}

std::unique_ptr<IScene> GameScene::Update() {

	debugCamera_->Update();
	camera_ = *static_cast<Camera*>(debugCamera_.get());

	return nullptr;
}

void GameScene::Draw() {
	display_->PreDraw(gameWindow_->GetCommandObject(), true);

	//Playerとかの処理

	display_->PostDraw(gameWindow_->GetCommandObject());

	//PostEffectとか

	gameWindow_->PreDraw();
	
	//ImGui

	gameWindow_->DrawDisplayWithImGui();
	paramManager_->Draw();

	engine_->ImGuiDraw();
}
