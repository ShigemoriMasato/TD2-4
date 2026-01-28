#include "GameScene.h"
#include <Common/DebugParam/GameParamEditor.h>
#include"FpsCount.h"
#include"GameCamera/DebugMousePos.h"
#include"Item/OreItemStorageNum.h"
#include"LightManager.h"
#include <Utility/Easing.h>

namespace {
	//MiniMap確認用
	bool poseMode = false;

	std::string fontName = "ZenOldMincho-Medium.ttf";

	std::string playerModelName = "Player";

	std::string oreModelName = "Ore";

	std::string mineralModelName = "Mineral";

	std::string spriteModelName = "Sprite";
}

void GameScene::Initialize() {
	gameWindow_ = commonData_->mainWindow.get();
	display_ = commonData_->display.get();
	//カメラの初期化
	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize();
	//パラメーター管理の初期化
	paramManager_ = std::make_unique<ParamManager>();

	//　当たり判定管理クラスを初期化
	colliderManager_ = std::make_unique<ColliderManager>();
	// 当たり判定管理クラスを登録
	Collider::SetColliderManager(colliderManager_.get());

	auto inst = GameParamEditor::GetInstance();
	inst->SetActiveScene("GameScene");
	inst->CreateGroup("Test", "GameScene");
	inst->AddItem("Test", "param1", param1, 0);
	inst->AddItem("Test", "param2", param2, 1);
	inst->AddItem("Test", "param3", param3, 2);
	inst->AddItem("Test", "param4", param4, 3);
	inst->AddItem("Test", "param5", param5, 4);

	// UIカメラを設定
	uiCamera_ = std::make_unique<Camera>();
	uiCamera_->SetProjectionMatrix(OrthographicDesc{});
	uiCamera_->MakeMatrix();

	// ライトの初期化
	LightManager::light_.color = { 1.0f,1.0f,1.0f,1.0f };
	LightManager::light_.direction = { 0.0f,-1.0f,0.0f };
	LightManager::light_.intensity = 1.0f;

	//==================================================
	// カメラシステム
	//==================================================

	// ゲームカメラを設定
	cameraController_ = std::make_unique<CameraController>();
	cameraController_->Initialize(input_);

	//============================================
	// マップシステム
	//============================================

	currentMap_ = commonData_->newMapManager->GetMapData(0);

	// マップデータ解釈機能を初期化
	mapChipField_ = std::make_unique<MapChipField>();
	// マップデータの受け取り
	mapChipField_->SetMapChipData(currentMap_.mapChipData);
	
	// マップの最大サイズを取得
	cameraController_->SetMapMaxSize(mapChipField_->GetMaxMapSize());


	// 壁モデルを取得
	int wallModelID = modelManager_->LoadModel(playerModelName);
	auto wallModel = modelManager_->GetNodeModelData(wallModelID);

	// マップの描画機能を初期化
	mapRender_ = std::make_unique<MapRender>();
	mapRender_->Initialize(drawDataManager_->GetDrawData(wallModel.drawDataIndex));
	mapRender_->SetConfig(currentMap_.renderData);

	//Debug用マップ描画の初期化
	debugMapRender_ = std::make_unique<DebugMCRender>();
	debugMapRender_->Initialize(drawDataManager_->GetDrawData(modelManager_->GetNodeModelData(0).drawDataIndex));
	debugMapRender_->SetAlpha(1.0f);

	//ColorMap作成
	LoadDebugColorMap();

	//================================================================
	// 鉱石システム
	//================================================================

	// 鉱石モデル
	int oreItemModelID = modelManager_->LoadModel(mineralModelName);
	auto oreItemModel = modelManager_->GetNodeModelData(oreItemModelID);

	// 鉱石のテクスチャを取得
	int oreItemTextrueIndex = textureManager_->GetTexture("Mineral-0.png");

	// 鉱石の管理システムを初期化
	oreItemManager_ = std::make_unique<OreItemManager>();
	oreItemManager_->Initialize(drawDataManager_->GetDrawData(oreItemModel.drawDataIndex), oreItemTextrueIndex);

	//============================================================================
	// ユニットシステム
	//============================================================================

	// プレイヤーモデルを取得
	int playerModelID = modelManager_->LoadModel(playerModelName);
	auto playerModel = modelManager_->GetNodeModelData(playerModelID);

	// プレイヤーのテクスチャを取得
	int playerTextureIndex = textureManager_->GetTexture("Player.png");

	// おれモデルを取得
	int oreModelID = modelManager_->LoadModel(oreModelName);
	auto oreModel = modelManager_->GetNodeModelData(oreModelID);

	// おれのテクスチャを取得
	int oreTextureIndex = textureManager_->GetTexture("untitled-0.png");

	// ユニットの管理クラス
	unitManager_ = std::make_unique<UnitManager>();
	unitManager_->Initalize(mapChipField_.get(),
		drawDataManager_->GetDrawData(playerModel.drawDataIndex), playerTextureIndex,
		drawDataManager_->GetDrawData(oreModel.drawDataIndex), oreTextureIndex,
		commonData_->keyManager.get());

	//==========================================================================================
	// ポストエフェクト
	//==========================================================================================

	postEffect_ = std::make_unique<PostEffect>();
	postEffect_->Initialize(textureManager_, drawDataManager_->GetDrawData(commonData_->postEffectDrawDataIndex));

	postEffectConfig_.window = gameWindow_->GetWindow();
	postEffectConfig_.origin = display_;
	postEffectConfig_.jobs_ = 0;

	//=======================================================
	// ゲームオーバーシーンの初期化
	//=======================================================
	InitializeGameOver();

	//==================================================================
	// UI
	//==================================================================

	// spriteモデルを取得
	int spriteModelID = modelManager_->LoadModel(spriteModelName);
	auto spriteModel = modelManager_->GetNodeModelData(spriteModelID);

	/// シーン遷移の処理
	fadeTransition_ = std::make_unique<FadeTransition>();
	fadeTransition_->Initialize(drawDataManager_->GetDrawData(spriteModel.drawDataIndex));

	auto drawData = drawDataManager_->GetDrawData(modelManager_->GetNodeModelData(1).drawDataIndex);

	// ユニットの数UI
	unitCounterUI_ = std::make_unique<CounterUI>();
	unitCounterUI_->Initialize(fontName, L"ユニット :", unitManager_->GetMaxOreCount(), unitManager_->GetMaxOreCount(), drawData, fontLoader_);
	unitCounterUI_->fontObject_->transform_.position.x = 800.0f;
	unitCounterUI_->fontObject_->transform_.position.y = 128.0f;

	// 鉱石のアイテムUI
	oreItemUI_ = std::make_unique<CounterUI>();
	oreItemUI_->Initialize(fontName, L"こうせき :", OreItemStorageNum::currentOreItemNum_, OreItemStorageNum::maxOreItemNum_, drawData, fontLoader_);
	oreItemUI_->fontObject_->transform_.position.x = 800.0f;
	oreItemUI_->fontObject_->transform_.position.y = 200.0f;

	// 時間を測る
	timeTracker_ = std::make_unique<TimeTracker>();
	timeTracker_->StartMeasureTimes();

	// 時間を表示するUI
	timerUI_ = std::make_unique<TimerUI>();
	timerUI_->Initialize(fontName, drawData, fontLoader_);
}

void GameScene::InitializeGameOver() {

	// spriteモデルを取得
	int spriteModelID = modelManager_->LoadModel(spriteModelName);
	auto spriteModel = modelManager_->GetNodeModelData(spriteModelID);

	auto drawData = drawDataManager_->GetDrawData(modelManager_->GetNodeModelData(1).drawDataIndex);

	// ゲームオーバーUIの初期化
	gameOverUI_ = std::make_unique<GameOverUI>();
	gameOverUI_->Initialize(drawDataManager_->GetDrawData(spriteModel.drawDataIndex), commonData_->keyManager.get(), fontName, drawData, fontLoader_);
	// リトライ
	gameOverUI_->SetOnRetryClicked([this]() {
		isSceneChange_ = true;
	});
	// タイトル
	gameOverUI_->SetOnSelectClicked([this]() {
		isSceneChange_ = true;
	});
}

std::unique_ptr<IScene> GameScene::Update() {

	// Δタイムを取得する
	FpsCount::deltaTime = engine_->GetFPSObserver()->GetDeltatime();

	// 入力処理の更新
	input_->Update();
	commonData_->keyManager->Update();

	debugCamera_->Update();
	camera_ = *static_cast<Camera*>(debugCamera_.get());

	//=============================================================
	// シーン遷移の管理
	//=============================================================

	// 演出が終わったので次のシーンに切り替える
	if (fadeTransition_->IsFinished()) {

	}

	if (fadeTransition_->IsAnimation()) {

		// シーン遷移
		fadeTransition_->Update();
	} else {
		if (isGameOverScene_) {

			// UIの更新処理
			gameOverUI_->Update();

		} else {
			// ゲームの更新処理
			InGameScene();
		}
	}

	// 切り替える
	if (isSceneChange_) {
		return std::make_unique<GameScene>();
	}

	return nullptr;
}

void GameScene::InGameScene() {

	//====================================================
	// カメラの更新庶路
	//====================================================

	// マウスのスクリーン座標を取得する
	POINT cursorPos;
	if (GetCursorPos(&cursorPos)) {
		// スクリーン座標をクライアント座標に変換
		ScreenToClient(gameWindow_->GetWindow()->GetHwnd(), &cursorPos);
		// カーソル位置をワールド座標に変換
		DebugMousePos::screenMousePos = { static_cast<float>(cursorPos.x), static_cast<float>(cursorPos.y) };
	}
	// カメラの更新処理
	cameraController_->Update();

	//==============================================================
	// ユニットの選択処理
	//==============================================================

	// マウスの位置に鉱石が存在していればユニットを動かす
	Vector3 oreWorldPos = {};
	if (oreItemManager_->IsSelectOre(cameraController_->GetWorldPos(), oreWorldPos)) {

		// 左クリックを取得
		if ((Input::GetMouseButtonState()[0] & 0x80) && !(Input::GetPreMouseButtonState()[0] & 0x80)) {
			// 選択された鉱石を取得
			OreItem* selectedOreItem = oreItemManager_->GetOreItemForId();

			// 追加出来るかを確認
			int32_t deletaNum = selectedOreItem->IsFullWorker(unitManager_->GetUnitSpawnNum());
			if (deletaNum >= 0) {

				// おれを追加
				unitManager_->RegisterUnit(selectedOreItem->GetPos(), 0, selectedOreItem);

				// 鉱石側の労働者カウントを増やす
				for (int i = 0; i < unitManager_->GetUnitSpawnNum(); ++i) {
					selectedOreItem->AddWorker();
				}
			} else {
				if (unitManager_->GetUnitSpawnNum() >= deletaNum * -1.0f) {
					// おれを追加
					unitManager_->RegisterUnit(selectedOreItem->GetPos(), deletaNum, selectedOreItem);

					// 鉱石側の労働者カウントを増やす
					int32_t actualSpawnCount = unitManager_->GetUnitSpawnNum() + deletaNum;
					for (int i = 0; i < actualSpawnCount; ++i) {
						selectedOreItem->AddWorker();
					}
				}
			}
		}
	}

	//=====================================================
	// 鉱石の更新処理
	//=====================================================

	// 鉱石管理の更新処理
	oreItemManager_->Update();

	//====================================================================
	// ユニットの更新処理
	//====================================================================

	// ユニットの更新処理
	unitManager_->Update();

	// 出撃出来るユニットがいなくなったらゲームオーバー
	if (unitManager_->GetMaxOreCount() <= 0) {
		isGameOverScene_ = true;
	}

	//============================================
	// 当たり判定
	//============================================

	// 全ての当たり判定を判定
	colliderManager_->CollisionCheckAll();

	//================================================
	// UIの更新処理
	//================================================

	// ユニットの数を更新
	unitCounterUI_->Update(unitManager_->GetMaxOreCount() - unitManager_->GetOreCount(), unitManager_->GetMaxOreCount());

	// 鉱石の数を更新
	oreItemUI_->Update(OreItemStorageNum::currentOreItemNum_, OreItemStorageNum::maxOreItemNum_);

	// 時間を更新
	timeTracker_->Update();

	// 時間表示UIを更新
	timerUI_->Update();
}

void GameScene::Draw() {
	display_->PreDraw(gameWindow_->GetCommandObject(), true);

	// カメラ行列
	Matrix4x4 vpMatrix = cameraController_->GetVpMatrix();
	LightManager::light_.cameraWorldPos = cameraController_->GetCameraWorldPos();

	// マップを描画
	mapRender_->Draw(vpMatrix, gameWindow_->GetWindow());

	// デバッグ用マップ描画
	debugMapRender_->Draw(vpMatrix, colorMap_, currentMap_.mapChipData, gameWindow_->GetWindow());

	// 鉱石の描画
	oreItemManager_->Draw(gameWindow_->GetWindow(), vpMatrix);

	// ユニットを描画
	unitManager_->Draw(gameWindow_->GetWindow(), vpMatrix);

	/// UIの描画処理
	vpMatrix = uiCamera_->GetVPMatrix();

	// ユニットの数UIを描画
	unitCounterUI_->Draw(gameWindow_->GetWindow(), vpMatrix);
	// 鉱石の数UIを描画
	oreItemUI_->Draw(gameWindow_->GetWindow(), vpMatrix);

	// 時間計測表示UI
	timerUI_->Draw(gameWindow_->GetWindow(), vpMatrix);

	// ゲームオーバーシーンの描画処理
	if (isGameOverScene_) {
		// UIの更新処理
		gameOverUI_->Draw(gameWindow_->GetWindow(), vpMatrix);
	}

	// シーン遷移の描画
	fadeTransition_->Draw(gameWindow_->GetWindow(), vpMatrix);

	display_->PostDraw(gameWindow_->GetCommandObject());

	//PostEffectとか
	postEffectConfig_.output = gameWindow_->GetDualDisplay();
	postEffect_->Draw(postEffectConfig_);

	gameWindow_->PreDraw(false);

	
	//ImGui

	gameWindow_->DrawDisplayWithImGui();
	paramManager_->Draw();

	// カメラのデバック情報
	cameraController_->DebugDraw();

	engine_->ImGuiDraw();
}

void GameScene::RegisterDebugParam() {

}

void GameScene::ApplyDebugParam() {

}

void GameScene::LoadDebugColorMap() {
	auto values = binaryManager_.Read("MapTypeEditorConfig");
	
	//colorMapの初期化
	for (int i = 0; i < int(TileType::Count); ++i) {
		Vector4 color = ConvertColor(lerpColor(0xff5500ff, 0x0055ffff, float(i) / (float(TileType::Count) - 1)));
		colorMap_[static_cast<TileType>(i)] = { color.x, color.y, color.z };
	}

	int index = 0;
	while (index < values.size()) {
		int tile = binaryManager_.Reverse<int>(values[index++].get());
		Vector3 color = binaryManager_.Reverse<Vector3>(values[index++].get());
		colorMap_[static_cast<TileType>(tile)] = color;
	}
}
