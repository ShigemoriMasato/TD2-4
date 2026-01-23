#include "MapEditScene.h"
#include <imgui/imgui.h>

MapEditScene::~MapEditScene() {
	//imguiの設定を戻す
	auto io = ImGui::GetIO();
	io.IniFilename = "Assets/.EngineResource/imgui.ini";
}

void MapEditScene::Initialize() {
	//いろいろと特殊なのでimguiの設定を分ける
	auto io = ImGui::GetIO();
	io.IniFilename = "Assets/.EngineResource/forMapEditor.ini";

	mapEditor_ = std::make_unique<MapEditor::Editor>();
	mapEditor_->Initialize(commonData_->mapDataManager.get());
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

	mapEditor_->DrawImGui();

	window.DrawDisplayWithImGui();
	engine_->ImGuiDraw();
#endif
}
