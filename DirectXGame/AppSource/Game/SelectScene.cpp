#include"SelectScene.h"
#include <Common/DebugParam/GameParamEditor.h>
#include"FpsCount.h"
#include"GameCamera/DebugMousePos.h"
#include"Utility/Easing.h"
#include<numbers>

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

	// 追従カメラ
	selectCamera_ = std::make_unique<SelectCamera>();
	selectCamera_->Initialize();

	// プレイヤーモデルを取得
	int playerModelID = modelManager_->LoadModel(playerModelName);
	auto playerModel = modelManager_->GetNodeModelData(playerModelID);

	// プレイヤーの描画オブジェクト
	playerObject_ = std::make_unique<PlayerUnitObject>();
	playerObject_->Initialize(drawDataManager_->GetDrawData(playerModel.drawDataIndex));
	playerObject_->transform_.position = { 0.0f,0.0f,0.0f };
	playerObject_->transform_.rotate.y = std::numbers::pi_v<float> / 2;
	currentDir_ = 1.0f;

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

	/// シーン遷移の処理
	fadeTransition_ = std::make_unique<FadeTransition>();
	fadeTransition_->Initialize(drawDataManager_->GetDrawData(spriteModel.drawDataIndex));

	// インゲームの処理
	InGameScene();
}

std::unique_ptr<IScene> SelectScene::Update() {

	// Δタイムを取得する
	FpsCount::deltaTime = engine_->GetFPSObserver()->GetDeltatime();

	// 入力処理の更新
	input_->Update();
	commonData_->keyManager->Update();

	debugCamera_->Update();
	camera_ = *static_cast<Camera*>(debugCamera_.get());

	// 演出が終わったので次のシーンに切り替える
	if (fadeTransition_->IsFinished()) {

	}

	if (fadeTransition_->IsAnimation()) {

		// シーン遷移
		fadeTransition_->Update();
	} else {
		// インゲームの処理
		InGameScene();
	}
	
	return nullptr;
}

void SelectScene::InGameScene() {
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
				selectDir_ = -1.0f;
				startRotY_ = playerObject_->transform_.rotate.y;
				endRotY_ = -std::numbers::pi_v<float> / 2;
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
				selectDir_ = 1.0f;
				startRotY_ = playerObject_->transform_.rotate.y;
				endRotY_ = std::numbers::pi_v<float> / 2;
			}
		}
	}

	//==============================================
	// プレイヤーの更新処理
	//==============================================

	// 移動速度
	Vector3 velocity = { 0.0f,0.0f,0.0f };

	if (isPlayerAnimation_) {
		timer_ += FpsCount::deltaTime / moveTime_;

		Vector3 prePos = playerObject_->transform_.position;

		// 回転
		if (currentDir_ != selectDir_) {
			if (timer_ <= 0.3f) {
				float localT = timer_ / 0.3f;
				playerObject_->transform_.rotate.y = lerp(startRotY_, endRotY_, localT, EaseType::EaseInOutCubic);
			} else {
				playerObject_->transform_.rotate.y = endRotY_;
			}
		}

		// 移動
		playerObject_->transform_.position = lerp(startPos_, endPos_, timer_, EaseType::EaseInOutCubic);

		velocity = playerObject_->transform_.position - prePos;

		if (timer_ >= 1.0f) {
			currentDir_ = selectDir_;
			timer_ = 0.0f;
			isPlayerAnimation_ = false;
			playerObject_->transform_.position = endPos_;
		}
	}

	// プレイヤーの更新処理
	playerObject_->Update();

	//=====================================================
	// カメラの更新処理
	//=====================================================

	// カメラの追跡位置を更新
	selectCamera_->SetTargetPos(playerObject_->transform_.position, velocity);
	selectCamera_->Update();

	//==================================================
	// UIの更新処理
	//==================================================

	// ステージ選択の更新処理
	selectStageUI_->Update();

}

void SelectScene::Draw() {
	display_->PreDraw(gameWindow_->GetCommandObject(), true);


	Matrix4x4 vpMatrix = selectCamera_->GetVpMatrix();

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

	// シーン遷移の描画
	fadeTransition_->Draw(gameWindow_->GetWindow(), vpMatrix);

	display_->PostDraw(gameWindow_->GetCommandObject());

	//PostEffectとか9i[

	postEffectConfig_.output = gameWindow_->GetDualDisplay();
	postEffect_->Draw(postEffectConfig_);

	gameWindow_->PreDraw();

	//ImGui
	gameWindow_->DrawDisplayWithImGui();
	paramManager_->Draw();

	selectCamera_->DrawImGui();

	engine_->ImGuiDraw();
}
