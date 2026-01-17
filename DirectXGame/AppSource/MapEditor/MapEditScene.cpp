#include "MapEditScene.h"

void MapEditScene::Initialize() {
	editor_ = std::make_unique<MapEditor::Editor>();
	editor_->Initialize(commonData_->mapDataManager.get());
}

std::unique_ptr<IScene> MapEditScene::Update() {
	engine_->GetInput()->Update();

	return std::unique_ptr<IScene>();
}

void MapEditScene::Draw() {
	auto& display = *commonData_->display.get();
	auto& window = *commonData_->mainWindow.get();

	display.PreDraw(window.GetCommandObject(), true);

	display.PostDraw(window.GetCommandObject());

	window.PreDraw();

#ifdef USE_IMGUI
	editor_->DrawImGui();

	window.DrawDisplayWithImGui();
	engine_->ImGuiDraw();
#endif
}
