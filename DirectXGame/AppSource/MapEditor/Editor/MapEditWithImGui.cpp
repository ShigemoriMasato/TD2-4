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
	
	int prevWidth = mapData.width;
	if (ImGui::DragInt("MapSize", &mapData.width, 1.0f, 0, 1024)) {
		for (int i = prevWidth - 1; i < prevWidth * mapData.height; i += prevWidth) {
			mapData.tileData.insert(mapData.tileData.begin() + i + (mapData.width - prevWidth) * (i / prevWidth),TileType::Air);
		}
	}

	ImGui::SameLine();

	if (ImGui::DragInt("/", &mapData.height, 1.0f, 0, 1024)) {
		mapData.tileData.resize(mapData.width * mapData.height, TileType::Air);
	}

	ImGui::End();

#endif
}

int EditConfig::RequestMap() {
	return requestMapID_;
}
