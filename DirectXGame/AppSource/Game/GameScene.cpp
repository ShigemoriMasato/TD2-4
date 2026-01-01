#include "GameScene.h"
#include <imgui/imgui.h>

GameScene::GameScene() {
	tetris_ = std::make_unique<Tetris>();
	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize();
	manualCamera_ = std::make_unique<Camera>();
	manualCamera_->SetProjectionMatrix(PerspectiveFovDesc());
	manualCamera_->MakeMatrix();

	worldCamera_ = debugCamera_.get();
}

void GameScene::Initialize() {
	keyCoating_ = std::make_unique<KeyCoating>(commonData_->keyManager.get());
	DrawData drawData = drawDataManager_->GetDrawData(commonData_->blockIndex);

	tetris_->Initialize(keyCoating_.get(), worldCamera_, drawData);
}

std::unique_ptr<IScene> GameScene::Update() {
	float deltaTime = 1.0f / 60.0f;

	input_->Update();
	commonData_->keyManager->Update();
	keyCoating_->Update(deltaTime);

	debugCamera_->Update();

	tetris_->Update(deltaTime);

	return nullptr;
}

void GameScene::Draw() {
	auto window = commonData_->mainWindow.get();
	auto display = commonData_->display.get();

	display->PreDraw(window->GetCommandList(), true);
	tetris_->Draw(window->GetWindow());
	display->PostDraw(window->GetCommandList());

	window->PreDraw();
	window->PostDraw();

	//ここ以外で記述する場合、ifdefを忘れないようにすること
#ifdef USE_IMGUI
	manualCamera_->DrawImGui();
	manualCamera_->MakeMatrix();
	tetris_->DrawImGui();

	ImGui::Begin("Debug");
	ImGui::Text("A : %d", Input::GetKeyState(DIK_A));
	ImGui::Text("Left : %d", commonData_->keyManager->GetKeyStates().at(Key::Left));
	ImGui::Text("LeftCoated : %d", keyCoating_->GetKeyStates().at(Key::Left));
	ImGui::End();
#endif

	engine_->ImGuiDraw();
}
