#include"SelectScene.h"
#include <Common/DebugParam/GameParamEditor.h>
#include"FpsCount.h"
#include"GameCamera/DebugMousePos.h"
#include"Utility/Easing.h"


namespace {
	//Minimap確認用
	bool poseMode = false;

	std::string playerModelName = "Player";

	std::string spriteModelName = "Sprite";
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

	// UIカメラを設定
	uiCamera_ = std::make_unique<Camera>();
	uiCamera_->SetProjectionMatrix(OrthographicDesc{});
	uiCamera_->MakeMatrix();

	// プレイヤーモデルを取得
	int playerModelID = modelManager_->LoadModel(playerModelName);
	auto playerModel = modelManager_->GetNodeModelData(playerModelID);

	// プレイヤーの描画オブジェクト
	playerObject_ = std::make_unique<PlayerUnitObject>();
	playerObject_->Initialize(drawDataManager_->GetDrawData(playerModel.drawDataIndex));
	playerObject_->transform_.position = { 0.0f,0.0f,0.0f };

	// spriteモデルを取得
	int spriteModelID = modelManager_->LoadModel(spriteModelName);
	auto spriteModel = modelManager_->GetNodeModelData(spriteModelID);

	// ステージのロゴ表示機能
	selectStageUI_ = std::make_unique<SelectStageUI>();
	selectStageUI_->Init(drawDataManager_->GetDrawData(spriteModel.drawDataIndex));

	// プレイヤーモデルを取得
	int stageModelID = modelManager_->LoadModel(playerModelName);
	auto stageModel = modelManager_->GetNodeModelData(stageModelID);

	// ステージオブジェクトを描画
	for (int i = 0; i < stagePointObjects_.size(); ++i) {
		stagePointObjects_[i] = std::make_unique<DefaultObject>();
		stagePointObjects_[i]->Initialize(drawDataManager_->GetDrawData(stageModel.drawDataIndex));
		stagePointObjects_[i]->transform_.position = { i * 5.0f,-1.5f,0.0f };
		stagePointObjects_[i]->transform_.scale = { 1.5f,0.2f,1.5f };
		stagePointObjects_[i]->material_.color = { 1.0f,0.0f,0.0f,1.0f };
		stagePointObjects_[i]->Update();
	}
}

std::unique_ptr<IScene> SelectScene::Update() {

	// Δタイムを取得する
	FpsCount::deltaTime = engine_->GetFPSObserver()->GetDeltatime();

	// 入力処理の更新
	input_->Update();
	commonData_->keyManager->Update();

	debugCamera_->Update();
	camera_ = *static_cast<Camera*>(debugCamera_.get());

	//==================================================
	// ステージの選択処理
	//==================================================

	// キーの状態を取得
	auto key = commonData_->keyManager->GetKeyStates();

	if (key[Key::LeftTri]) {
		if (!selectStageUI_->IsAnimation() && !isPlayerAnimation_) {
			if (selectStageNum_ > 1) {
				selectStageNum_ -= 1;
				selectStageUI_->SetAnimation();
				// プレイヤー関係
				isPlayerAnimation_ = true;
				startPos_ = playerObject_->transform_.position;
				endPos_ = stagePointObjects_[selectStageNum_ - 1]->transform_.position;
				endPos_.y = 0.0f;
			}
		}
	}

	if (key[Key::RightTri]) {
		if (!selectStageUI_->IsAnimation() && !isPlayerAnimation_) {
			if (selectStageNum_ < maxStageNum_) {
				selectStageNum_ += 1;
				selectStageUI_->SetAnimation();
				// プレイヤー関係
				isPlayerAnimation_ = true;
				startPos_ = playerObject_->transform_.position;
				endPos_ = stagePointObjects_[selectStageNum_ - 1]->transform_.position;
				endPos_.y = 0.0f;
			}
		}	
	}

	//==============================================
	// プレイヤーの更新処理
	//==============================================

	if (isPlayerAnimation_) {
		timer_ += FpsCount::deltaTime / moveTime_;

		playerObject_->transform_.position = lerp(startPos_, endPos_, timer_, EaseType::EaseInOutCubic);

		if (timer_ >= 1.0f) {
			timer_ = 0.0f;
			isPlayerAnimation_ = false;
			playerObject_->transform_.position = endPos_;
		}
	}

	// プレイヤーの更新処理
	playerObject_->Update();

	//==================================================
	// UIの更新処理
	//==================================================

	// ステージ選択の更新処理
	selectStageUI_->Update();

	return nullptr;
}

void SelectScene::Draw() {
	display_->PreDraw(gameWindow_->GetCommandObject(), true);


	Matrix4x4 vpMatrix = debugCamera_->GetVPMatrix();

	// ステージを描画
	for (auto& point : stagePointObjects_) {
		point->Draw(gameWindow_->GetWindow(), vpMatrix);
	}

	// プレイヤーの描画
	playerObject_->Draw(gameWindow_->GetWindow(),vpMatrix);

	// UIの描画(ポストエフェクトなどを掛ける場合、掛からないように描画場所を変更する)

	vpMatrix = uiCamera_->GetVPMatrix();

	// ステージの選択
	selectStageUI_->Draw(gameWindow_->GetWindow(), vpMatrix);

	display_->PostDraw(gameWindow_->GetCommandObject());

	//PostEffectとか9i[

	postEffectConfig_.output = gameWindow_->GetDualDisplay();
	postEffect_->Draw(postEffectConfig_);

	gameWindow_->PreDraw();

	//ImGui
	gameWindow_->DrawDisplayWithImGui();
	paramManager_->Draw();

	engine_->ImGuiDraw();
}
