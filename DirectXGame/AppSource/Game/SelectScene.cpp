#include"SelectScene.h"
#include <Common/DebugParam/GameParamEditor.h>
#include"FpsCount.h"
#include"GameCamera/DebugMousePos.h"
#include"Utility/Easing.h"
#include<numbers>
#include"LightManager.h"
#include"SelectStageNum.h"
#include <Game/GameScene.h>

namespace {
	//Minimap確認用
	bool poseMode = false;

	std::string fontName = "YDWbananaslipplus.otf";

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
	
	// プレイヤーのテクスチャを取得
	int playerTextureIndex = textureManager_->GetTexture("Player.png");

	// プレイヤーの描画オブジェクト
	playerObject_ = std::make_unique<PlayerUnitObject>();
	playerObject_->Initialize(drawDataManager_->GetDrawData(playerModel.drawDataIndex), playerTextureIndex);
	playerObject_->transform_.position = { 0.0f,0.0f,-0.5f };
	playerObject_->transform_.rotate.y = std::numbers::pi_v<float> / 2;
	currentDir_ = 1.0f;

	// spriteモデルを取得
	int spriteModelID = modelManager_->LoadModel(spriteModelName);
	auto spriteModel = modelManager_->GetNodeModelData(spriteModelID);

	auto drawData = drawDataManager_->GetDrawData(modelManager_->GetNodeModelData(1).drawDataIndex);

	// ステージのロゴ表示機能
	selectStageUI_ = std::make_unique<SelectStageUI>();
	selectStageUI_->Init(drawDataManager_->GetDrawData(spriteModel.drawDataIndex), fontName, drawData, fontLoader_);

	// プレイヤーモデルを取得
	int stageModelID = modelManager_->LoadModel(playerModelName);
	auto stageModel = modelManager_->GetNodeModelData(stageModelID);

	// ステージオブジェクトを描画
	for (int i = 0; i < stagePointObjects_.size(); ++i) {
		stagePointObjects_[i] = std::make_unique<DefaultObject>();
		stagePointObjects_[i]->Initialize(drawDataManager_->GetDrawData(stageModel.drawDataIndex),0);
		stagePointObjects_[i]->transform_.position = { i * 6.0f,-1.5f,0.0f };
		stagePointObjects_[i]->transform_.scale = { 1.5f,0.2f,1.5f };
		stagePointObjects_[i]->material_.color = { 1.0f,0.0f,0.0f,1.0f };
		stagePointObjects_[i]->Update();
	}

	// 洞窟
	int cave1ModelID = modelManager_->LoadModel("Cave1");
	auto cave1Model = modelManager_->GetNodeModelData(cave1ModelID);

	int cave2ModelID = modelManager_->LoadModel("Cave2");
	auto cave2Model = modelManager_->GetNodeModelData(cave2ModelID);

	int cave3ModelID = modelManager_->LoadModel("Cave3");
	auto cave3Model = modelManager_->GetNodeModelData(cave3ModelID);

	// 洞窟テクスチャ
	int cave1Index = textureManager_->GetTexture("Cave_01.png");
	int cave2Index = textureManager_->GetTexture("Cave_02.png");
	int cave3Index = textureManager_->GetTexture("Cave_03.png");

	// 洞窟オブジェクトを描画
	for (int i = 0; i < caveObjects_.size(); ++i) {
		caveObjects_[i] = std::make_unique<DefaultObject>();
		if (i == 0) {
			caveObjects_[i]->Initialize(drawDataManager_->GetDrawData(cave1Model.drawDataIndex), cave1Index);
		} else if (i == 1) {
			caveObjects_[i]->Initialize(drawDataManager_->GetDrawData(cave2Model.drawDataIndex), cave2Index);
		} else {
			caveObjects_[i]->Initialize(drawDataManager_->GetDrawData(cave3Model.drawDataIndex), cave3Index);
		}
		caveObjects_[i]->transform_.position = { i * 6.0f - 1.0f,3.0f,2.5f };
		caveObjects_[i]->transform_.scale = { 1.0f,1.0f,1.0f };
		caveObjects_[i]->material_.color = { 1.0f,1.0f,1.0f,1.0f };
		caveObjects_[i]->material_.isActive = true;
		caveObjects_[i]->Update();
	}

	// 足場オブジェクト
	int selectModelID = modelManager_->LoadModel("Select");
	auto selectModel = modelManager_->GetNodeModelData(selectModelID);

	// 足場テクスチャ
	int selectIndex = textureManager_->GetTexture("Select-0.png");

	// 足場オブジェクト
	floorObject_ = std::make_unique<DefaultObject>();
	floorObject_->Initialize(drawDataManager_->GetDrawData(selectModel.drawDataIndex), selectIndex);
	floorObject_->transform_.position = {4.3f,17.0f,4.8f };
	floorObject_->transform_.scale = { 3.8f,3.0f,3.0f };
	floorObject_->material_.color = { 1.0f,1.0f,1.0f,1.0f };
	floorObject_->material_.isActive = true;
	floorObject_->Update();

	/// シーン遷移の処理
	fadeTransition_ = std::make_unique<FadeTransition>();
	fadeTransition_->Initialize(drawDataManager_->GetDrawData(spriteModel.drawDataIndex));

	// インゲームの処理
	InGameScene();

	//=================================================================
	// 
	// タイトルシーンの初期化
	// 
	//=================================================================

	// タイトルロゴを描画
	titleSprite_ = std::make_unique<SpriteObject>();
	titleSprite_->Initialize(drawDataManager_->GetDrawData(spriteModel.drawDataIndex), { 400.0f,128.0f });
	titleSprite_->transform_.position = { 640.0f,280.0f,0.0f };
	titleSprite_->color_ = { 1.0f,1.0f,1.0f,1.0f };
	titleSprite_->Update();

	// プレススペース
	spaceSprite_ = std::make_unique<SpriteObject>();
	spaceSprite_->Initialize(drawDataManager_->GetDrawData(spriteModel.drawDataIndex), { 256.0f,64.0f });
	spaceSprite_->transform_.position = { 640.0f,500.0f,0.0f };
	spaceSprite_->color_ = { 1.0f,1.0f,1.0f,1.0f };
	spaceSprite_->Update();

	// タイトルカメラ
	titleCamera_ = std::make_unique<TitleCamera>();
	titleCamera_->Initialize();

	if (commonData_->isTitle_) {
		isTitle_ = false;
	}

	commonData_->isTitle_ = true;

	spaceSprite_->Update();
	titleSprite_->Update();
	titleCamera_->Update();
	TitleScene();
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
		isSceneChange_ = false;
		// シーン遷移
		fadeTransition_->Update();
		isInPlayerAnimation_ = false;
	} else {

		if (isTitle_) {
			// タイトルシーンの更新処理
			TitleScene();
		} else {
			// インゲームの更新処理
			InGameScene();
		}	
	}

	// シーンを切り替える
	if (isSceneChange_ && !fadeTransition_->IsAnimation()) {
		return std::make_unique<GameScene>();
	}
	
	return nullptr;
}

void SelectScene::InGameScene() {
	//==================================================
	// ステージの選択処理
	//==================================================

	if (!isInPlayerAnimation_ || !fadeTransition_->IsAnimation()) {
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

		SelectStageNum::num_ = selectStageNum_;

		// 決定
		if (key[Key::Decision]) {
			commonData_->nextStageIndex = selectStageNum_ - 1;
			commonData_->nextMapIndex = 0;
			isInPlayerAnimation_ = true;
			inPlayerStartRotY_ = playerObject_->transform_.rotate.y;
		}
	}

	//==============================================
	// プレイヤーの更新処理
	//==============================================

	// 移動速度
	Vector3 velocity = { 0.0f,0.0f,0.0f };


	if (isInPlayerAnimation_) {

		inPlayerTimer_ += FpsCount::deltaTime / inPlayerTime_;

		if (inPlayerTimer_ <= 0.3f) {
			float localT = inPlayerTimer_ / 0.3f;
			playerObject_->transform_.rotate.y = lerp(inPlayerStartRotY_, 0.0f, localT, EaseType::EaseInOutCubic);
		}

		// 移動
		playerObject_->transform_.position.z = lerp(-0.5f, 4.0f, inPlayerTimer_, EaseType::EaseInOutCubic);

		if (inPlayerTimer_ >= 1.0f) {
			isSceneChange_ = true;
		}
	} else {
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
	}

	// プレイヤーの更新処理
	playerObject_->Update();


	floorObject_->Update();

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

void SelectScene::TitleScene() {

	// キーの状態を取得
	auto key = commonData_->keyManager->GetKeyStates();

	if (key[Key::Decision]) {
		isStartTitle_ = true;
	}

	if (!isStartTitle_) { return; }

	titleTimer_ += FpsCount::deltaTime / titleTime_;

	// 移動
	if (titleTimer_ <= 1.0f) {
		titleSprite_->transform_.position.y = lerp(280.0f, -360.0f, titleTimer_, EaseType::EaseInOutCubic);
	}

	if (titleTimer_ <= 0.5f) {
		float localT = titleTimer_ / 0.5f;
		spaceSprite_->color_.w = lerp(1.0f, 0.0f, localT, EaseType::EaseInOutCubic);
	}
	spaceSprite_->Update();

	// 更新処理
	titleSprite_->Update();

	// カメラの更新処理
	titleCamera_->Update();

	if (titleTimer_ >= 1.0f) {

		if (!titleCamera_->IsAnimation()) {
			isTitle_ = false;
		}
	}
}

void SelectScene::Draw() {
	display_->PreDraw(gameWindow_->GetCommandObject(), true);

	Matrix4x4 vpMatrix = selectCamera_->GetVpMatrix();
	if (isTitle_) {
		vpMatrix = titleCamera_->GetVpMatrix();
	}

	// ステージを描画
	for (auto& point : stagePointObjects_) {
		point->Draw(gameWindow_->GetWindow(), vpMatrix);
	}

	// 足場を描画
	floorObject_->Draw(gameWindow_->GetWindow(), vpMatrix);

	// 洞窟を描画
	for (int i = 0; i < caveObjects_.size(); ++i) {
		caveObjects_[i]->Draw(gameWindow_->GetWindow(), vpMatrix);
	}

	// プレイヤーの描画
	playerObject_->Draw(gameWindow_->GetWindow(),vpMatrix);

	// UIの描画(ポストエフェクトなどを掛ける場合、掛からないように描画場所を変更する)

	vpMatrix = uiCamera_->GetVPMatrix();

	if (isTitle_) {
		// タイトルロゴの描画
		titleSprite_->Draw(gameWindow_->GetWindow(), vpMatrix);

		// 操作UI
		spaceSprite_->Draw(gameWindow_->GetWindow(), vpMatrix);
	} else {
		// ステージの選択
		selectStageUI_->Draw(gameWindow_->GetWindow(), vpMatrix);
	}

	// シーン遷移の描画
	fadeTransition_->Draw(gameWindow_->GetWindow(), vpMatrix);

	display_->PostDraw(gameWindow_->GetCommandObject());

	//PostEffectとか9i[

	postEffectConfig_.output = gameWindow_->GetDualDisplay();
	postEffect_->Draw(postEffectConfig_);

	gameWindow_->PreDraw(true);

	//ImGui
	gameWindow_->DrawDisplayWithImGui();
	paramManager_->Draw();

	selectCamera_->DrawImGui();

#ifdef USE_IMGUI
	ImGui::Begin("Titel");
	ImGui::DragFloat3("pos", &floorObject_->transform_.position.x, 0.1f);
	ImGui::DragFloat3("size", &floorObject_->transform_.scale.x, 0.1f);
	ImGui::End();
#endif
	engine_->ImGuiDraw();
}
