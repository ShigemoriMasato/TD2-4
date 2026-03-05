#include "MiharaScene.h"
#include <Utility/Color.h>
#include <Utility/MatrixFactory.h>
#include <imgui/imgui.h>

void MiharaScene::Initialize() {
	// プレイヤーの生成&初期化
	player_ = std::make_unique<Player::Base>();
	player_->Initialize(modelManager_, drawDataManager_, input_);

	// プレイヤーHPの生成&初期化
	playerHP_ = std::make_unique<Player::HP>();
	playerHP_->Initialize(modelManager_, drawDataManager_, input_, player_->GetStatusManager());

	// プレイヤーのレベルシステムの生成&初期化
	playerLevelSystem_ = std::make_unique<Player::LevelSystem>();
	playerLevelSystem_->Initialize();

	// プレイヤーのレベルUIの生成&初期化
	playerLevelUI_ = std::make_unique<Player::LevelUI>();
	playerLevelUI_->Initialize(modelManager_, drawDataManager_, input_);

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
	weaponManager_->InitializeData(modelManager_,drawDataManager_);

	// 武器のパラメータ管理デバッガーの生成&初期化
	weaponDebugger_ = std::make_unique<WeaponDebugger>(weaponManager_.get());

	// カメラのトランスフォーム設定
	cameraTransform_.position = cameraOffset_;
	cameraTransform_.rotate = {-0.65f, 0.0f, 0.0f};
	cameraTransform_.scale = {1.0f, 1.0f, 1.0f};

	// ウェーブ中にどれくらいレベルが上がったかを管理するインスタンスの生成&初期化
	levelProgressTracker_ = std::make_unique<LevelProgressTracker>();

	// プレイヤーのヒートマップマネージャの生成&初期化
	playerHeatmapManager_ = std::make_unique<Player::HeatmapManager>();
	playerHeatmapManager_->Initialize(modelManager_, drawDataManager_, config_);

	// フィールドの生成&初期化
	field_ = std::make_unique<Field>();
	field_->Initialize(modelManager_, drawDataManager_);

	// グリッド
	grid_ = std::make_unique<Grid>();
	grid_->Initialize(drawDataManager_);

	// 敵管理クラスの生成&初期化
	enemyManager_ = std::make_unique<EnemyManager>();

	// 武器のコントローラーの生成&初期化
	
}

std::unique_ptr<IScene> MiharaScene::Update() {
	float deltaTime = engine_->GetDeltaTime();

	// ポーズ状態の切り替え
	if (input_->GetKeyState(DIK_P) && !input_->GetPreKeyState(DIK_P)) {
		isPaused_ = !isPaused_;
	}

	// ポーズ中でないときのみ実行
	if (!isPaused_) {
		Matrix4x4 cameraMatrix = Matrix::MakeAffineMatrix(cameraTransform_.scale, cameraTransform_.rotate, cameraTransform_.position);
		camera_->SetTransform(cameraMatrix);
		camera_->MakeMatrix();

		// プレイヤーの更新
		player_->Update(camera_->GetVPMatrix(), deltaTime);

		// プレイヤーのHP更新
		playerHP_->Update(camera_->GetVPMatrix(), deltaTime);

#ifdef _DEBUG
		// 経験値ゲージの増加 デバッグ用
		if (input_->GetKeyState(DIK_3)) {
			playerLevelSystem_->AddExp(1.0f);
		}
#endif // DEBUG

		// プレイヤーのレベルUIの更新
		playerLevelUI_->Update(camera_->GetVPMatrix(), deltaTime, playerLevelSystem_->GetCurrentExp(), playerLevelSystem_->GetNextExp());

		// プレイヤーのヒートマップ更新処理
		playerHeatmapManager_->Update(camera_->GetVPMatrix());

		// プレイヤーの滞在時間を記録
		playerHeatmapManager_->Record(player_->GetTransform().position, deltaTime);

		// フィールドの更新
		field_->Update(camera_->GetVPMatrix());

		// グリッドの更新
		grid_->Update(cameraTransform_.position, camera_->GetVPMatrix());

		// 武器のコントローラーの更新
		
	}

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

	// プレイヤーのEXPの描画
	playerLevelUI_->Draw(cmdObj);

	// ヒートマップの描画
	playerHeatmapManager_->Draw(cmdObj);

	// フィールドの描画
	field_->Draw(cmdObj);

	// グリッドの描画
	if (showGrid_) {
		grid_->Draw(cmdObj);
	}

	// 武器の描画
	

	display->PostDraw(cmdObj);

	window->PreDraw(cmdObj);

	// ここ以外で記述する場合、ifdefを忘れないようにすること
#ifdef USE_IMGUI

	display->DrawImGui();

	// ポーズ中に表示するImGui
	DrawImGuiPause();

	// デバッグ機能ImGui表示
	DrawDebugUI();

	// カメラのImGui表示
	DrawImGuiCamera();

	// プレイヤーのレベルシステム ImGui描画
	playerLevelSystem_->DrawImGui();

	// 武器のパラメータ管理デバッグ用描画
	weaponDebugger_->Draw();
#endif

	engine_->DrawImGui();
	window->PostDraw(cmdObj);
}

void MiharaScene::DrawDebugUI() {
#ifdef USE_IMGUI
	ImGui::Begin("Debug");

	// FPS等
	float dt = engine_->GetFPSObserver()->GetDeltatime();
	ImGui::Text("DeltaTime: %.3f ms", dt * 1000.0f);
	ImGui::Text("FPS: %.1f", 1.0f / dt);

	// グリッド
	ImGui::Checkbox("Show Grid", &showGrid_);
	ImGui::SameLine();
	ImGui::TextColored(showGrid_ ? ImVec4(0, 1, 0, 1) : ImVec4(1, 0, 0, 1), showGrid_ ? "ON" : "OFF");

	// 基本操作
	ImGui::Text("操作方法");
	ImGui::Text("WASDで移動");
	ImGui::Text("Spaceでダッシュ");
	ImGui::Text("Pでポーズ");

	ImGui::End();
#endif
}

void MiharaScene::DrawImGuiCamera() {
#ifdef USE_IMGUI
	// カメラのImGUi
	ImGui::Begin("Camera");
	ImGui::DragFloat3("Scale", &cameraTransform_.scale.x, 0.01f);
	ImGui::DragFloat3("Rotate", &cameraTransform_.rotate.x, 0.01f);
	ImGui::DragFloat3("Position", &cameraTransform_.position.x, 0.01f);
	ImGui::End();
#endif
}

void MiharaScene::DrawImGuiPause() {
#ifdef USE_IMGUI
	// ポーズ中ImGui表示
	if (isPaused_) {
		ImGui::Begin("ポーズメニュー", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Text("=== ポーズ中 ===");
		ImGui::Text("Pキーで戻る");
		ImGui::End();
	}
#endif
}
