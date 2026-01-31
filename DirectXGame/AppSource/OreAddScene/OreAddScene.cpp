#include "OreAddScene.h"

void OreAddScene::Initialize() {
}

std::unique_ptr<IScene> OreAddScene::Update() {
	input_->Update();



	return std::unique_ptr<IScene>();
}

void OreAddScene::Draw() {

	GameWindow* window = commonData_->mainWindow.get();
	DualDisplay* display = commonData_->display.get();

	display->PreDraw(window->GetCommandObject(), true);

	//描画処理


	display->PostDraw(window->GetCommandObject());

	//PostEffect


	//もしSwapChainに直接書き込むならココ
	window->PreDraw(true);


#ifdef USE_IMGUI

	//ImGui


	//ImGuiの最終処理
	window->DrawDisplayWithImGui();
	engine_->ImGuiDraw();
#endif
}
