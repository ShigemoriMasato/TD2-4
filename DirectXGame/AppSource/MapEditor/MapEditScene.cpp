#include "MapEditScene.h"

void MapEditScene::Initialize() {
}

std::unique_ptr<IScene> MapEditScene::Update() {
	return std::unique_ptr<IScene>();
}

void MapEditScene::Draw() {
	auto& display = *commonData_->display.get();
	auto& window = *commonData_->mainWindow.get();

	display.PreDraw(window.GetCommandList(), true);

	display.PostDraw(window.GetCommandList());

	window.PreDraw();

#ifdef USE_IMGUI


	window.DrawDisplayWithImGui();
	engine_->ImGuiDraw();
#endif
}
