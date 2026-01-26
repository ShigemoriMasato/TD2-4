#include "MapTypeEditor.h"
#include <cassert>
#include <GameCamera/DebugMousePos.h>

void MapTypeEditor::Initialize(MapChipData* mcData, Input* input) {
	assert(mcData);
	assert(input);
	mcData_ = mcData;
	input_ = input;

	tileStrings_ = {
		{TileType::Air, "Air"},
		{TileType::Road, "Road"},
		{TileType::Wall, "Wall"},
		{TileType::Home, "Home"},
		{TileType::Gold, "Gold"}
	};
}

void MapTypeEditor::Update() {
	//クリックされたら変更
	if (input_->GetMouseButtonState()[0] && DebugMousePos::isHovered) {
		std::pair<int, int> gridedPos = {
			static_cast<int>(cursorPos_.x),
			static_cast<int>(cursorPos_.y)
		};

		if(mcData_->size() <= gridedPos.second || mcData_->at(0).size() <= gridedPos.first){
			return;
		}

		(*mcData_)[gridedPos.second][gridedPos.first] = currentType_;
	}
}

void MapTypeEditor::DrawImGui(std::map<TileType, Vector3> colorMap) {
#ifdef USE_IMGUI

	ImGui::Begin("MapChip");

	for (int i = 0; i < int(TileType::Count); ++i) {
		ImGui::PushID(i);

		//各プロパティの設定
		TileType type = TileType(i);
		Vector3 color = colorMap[type];
		ImVec2 size = ImVec2(50, 50);
		if (type == currentType_) {
			size = ImVec2(60, 60);
		}

		//ボタン
		if (ImGui::ColorButton("a", ImVec4(color.x, color.y, color.z, 1.0f), 0, size)) {
			currentType_ = type;
			someSelected_ = true;
		}
		ImGui::SameLine();
		ImGui::Text("%s", tileStrings_[type].c_str());
		ImGui::PopID();
	}

	ImGui::End();

	//マップサイズ変更
	ImGui::Begin("MapEdit");
	ImGui::DragInt("width", &width_, 1.0f, 0, 128);
	ImGui::DragInt("height", &height_, 1.0f, 0, 128);
	ImGui::End();
	mcData_->resize(height_);
	for (auto& row : *mcData_) {
		row.resize(width_);
	}

#endif // USE_IMGUI
}
