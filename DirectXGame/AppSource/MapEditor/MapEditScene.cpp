#include "MapEditScene.h"
#include <imgui/imgui.h>

MapEditScene::~MapEditScene() {
	//imguiの設定を戻す
#ifdef USE_IMGUI
	auto io = ImGui::GetIO();
	io.IniFilename = "Assets/.EngineResource/imgui.ini";
#endif
}

void MapEditScene::Initialize() {
	//いろいろと特殊なのでimguiの設定を分ける
#ifdef USE_IMGUI
	auto io = ImGui::GetIO();
	io.IniFilename = "Assets/.EngineResource/forMapEditor.ini";
#endif

	mapEditor_ = std::make_unique<MapEditor::Editor>();
	mapEditor_->Initialize(commonData_->mapDataManager.get());

	mapModelEditor_ = std::make_unique<MapModelEditor>();
	mapModelEditor_->Initialize(modelManager_, drawDataManager_);
}

std::unique_ptr<IScene> MapEditScene::Update() {
	engine_->GetInput()->Update();

	mapEditor_->Update();
	mapModelEditor_->Update();

	return std::unique_ptr<IScene>();
}

void MapEditScene::Draw() {
	auto& display = *commonData_->display.get();
	auto& window = *commonData_->mainWindow.get();

	display.PreDraw(window.GetCommandObject(), true);

	mapModelEditor_->Draw(window.GetWindow());

	display.PostDraw(window.GetCommandObject());

	window.PreDraw(true);

#ifdef USE_IMGUI

	ImGui::Begin("EditorScene");
	if (type_ == 0) {
		ImGui::Text("Map Editor");
	} else if (type_ == 1) {
		ImGui::Text("Map Model Editor");
	}
	ImGui::SameLine();
	if (ImGui::Button("-")) {
		--type_;
		if (type_ < 0) {
			type_ = 1;
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("+")) {
		++type_;
		if (type_ > 1) {
			type_ = 0;
		}
	}
	ImGui::End();

	if (type_ == 0) {
		mapEditor_->DrawImGui();
	} else if (type_ == 1) {
		mapModelEditor_->DrawImGui();
	}

	window.DrawDisplayWithImGui();
	engine_->ImGuiDraw();
#endif
}
