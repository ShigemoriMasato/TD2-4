#include "MapEditWithImGui.h"
#include <algorithm>

using namespace MapEditor;

void EditConfig::Initialize() {
}

void EditConfig::DrawImGui(MapDataForBin& mapData) {
#ifdef USE_IMGUI

	ImGui::Begin("Map Config");

	ImGui::Text("Map ID: %d", mapData.mapID);
	ImGui::SameLine();
	if (ImGui::Button("-")) {
		requestMapID_ = std::max(0, mapData.mapID - 1);
	}
	ImGui::SameLine();
	if (ImGui::Button("+")) {
		requestMapID_ = mapData.mapID + 1;
	}

	ImGui::Separator();

	ImGui::InputText("Model File Path", modelFilePathBuffer_, 256);
	if (ImGui::Button("Load")) {
		mapData.modelFilePath = modelFilePathBuffer_;
	}
	
	if (ImGui::DragInt2("Width/Height", &mapData.width, 1.0f, 0, 1024)) {
		mapData.tileData.resize(mapData.width * mapData.height);
	}

	ImGui::End();

#endif
}

int EditConfig::RequestMap() {
	return requestMapID_;
}
