#include "MapTypeEditor.h"
#include <cassert>
#include <GameCamera/DebugMousePos.h>
#include <Utility/Color.h>
#include <Utility/Easing.h>
#include <imgui/imgui.h>

void MapTypeEditor::Initialize(Input* input) {
	assert(input);
	input_ = input;

	tileStrings_ = {
		{TileType::Air, "Air"},
		{TileType::Road, "Road"},
		{TileType::Wall, "Wall"},
		{TileType::Home, "Home"},
		{TileType::Gold, "Gold"}
	};

	Load();
}

void MapTypeEditor::Update() {
	someSelected_ = false;
	if (!editing_) {
		return;
	}

	//クリックされたら変更
	if (input_->GetMouseButtonState()[0] && DebugMousePos::isHovered) {
		std::pair<int, int> gridedPos = {
			static_cast<int>(cursorPos_.x),
			static_cast<int>(cursorPos_.y)
		};

		if (mcData_[currentStage_].size() <= gridedPos.second || mcData_[currentStage_].at(0).size() <= gridedPos.first) {
			return;
		}

		(mcData_[currentStage_])[gridedPos.second][gridedPos.first] = currentType_;
	}
}

void MapTypeEditor::DrawImGui() {
#ifdef USE_IMGUI

	if (ImGui::Begin("MapChip")) {
		if (!editing_) {
			editing_ = true;
			someSelected_ = true;
		}
	}

	for (int i = 0; i < int(TileType::Count); ++i) {
		ImGui::PushID(i);

		//各プロパティの設定
		TileType type = TileType(i);
		Vector3& color = colorMap_[type];
		ImVec2 size = ImVec2(50, 50);
		if (type == currentType_) {
			size = ImVec2(60, 60);
		}

		//ボタン
		if (ImGui::ColorButton(tileStrings_[type].c_str(), ImVec4(color.x, color.y, color.z, 1.0f), 0, size)) {
			currentType_ = type;
		}
		ImGui::SameLine();
		ImGui::Text("%s", tileStrings_[type].c_str());

		ImGui::ColorEdit3("##color", &color.x);

		ImGui::PopID();
	}

	ImGui::End();

	//マップサイズ変更
	ImGui::Begin("MapEdit");
	ImGui::DragInt("width", &width_, 1.0f, 0, 128);
	ImGui::DragInt("height", &height_, 1.0f, 0, 128);
	ImGui::End();
	mcData_[currentStage_].resize(height_);
	for (auto& row : mcData_[currentStage_]) {
		row.resize(width_);
	}

#endif // USE_IMGUI
}

void MapTypeEditor::SetCurrentStage(int currentStage) {
	currentStage_ = currentStage;
	if (mcData_.size() <= currentStage_) {
		mcData_.resize(currentStage_ + 1);
	}

	width_ = int(mcData_[currentStage_].empty() ? 0 : mcData_[currentStage_].front().size());
	height_ = int(mcData_[currentStage_].size());
}

void MapTypeEditor::Load() {
	auto values = binaryManager_.Read(saveStageFileName_);

	if (values.empty()) {
		mcData_.push_back({});
		goto EditorConfigLoad;
	}

	{
		int i = -1;
		for (size_t index = 0; index < values.size(); index) {
			++i;
			mcData_.emplace_back();
			int width = binaryManager_.Reverse<int>(values[index++].get());
			int height = binaryManager_.Reverse<int>(values[index++].get());

			if (i == 0) {
				width_ = width;
				height_ = height;
			}

			mcData_[i].resize(height, std::vector<TileType>(width));
			for (size_t y = 0; y < height; ++y) {
				for (size_t x = 0; x < width; ++x) {
					mcData_[i][y][x] = TileType(binaryManager_.Reverse<int>(values[index++].get()));
				}
			}
		}
	}

EditorConfigLoad:

	values = binaryManager_.Read(saveEditorConfigFileName_);

	if (values.empty()) {
		for (int i = 0; i < int(TileType::Count); ++i) {
			TileType type = TileType(i);
			Vector4 color = ConvertColor(lerpColor(0xff5500ff, 0x0055ffff, float(i) / float(TileType::Count)));
			colorMap_[type] = { color.x, color.y, color.z };
		}
		return;
	}

	for (size_t index = 0; index < values.size(); index) {
		TileType type = TileType(binaryManager_.Reverse<int>(values[index++].get()));
		Vector3 color = binaryManager_.Reverse<Vector3>(values[index++].get());
		colorMap_[type] = color;
	}
}

void MapTypeEditor::Save() {
	for (const auto& mc : mcData_) {
		int height = int(mc.size());
		int width = 0;

		if (height > 0) {
			width = int(mc.front().size());
		}

		binaryManager_.RegisterOutput(width);
		binaryManager_.RegisterOutput(height);

		for (int i = 0; i < height; ++i) {
			for (int j = 0; j < width; ++j) {
				binaryManager_.RegisterOutput(int(mc[i][j]));
			}
		}
	}

	binaryManager_.Write(saveStageFileName_);

	for (const auto& [type, color] : colorMap_) {
		binaryManager_.RegisterOutput(int(type));
		binaryManager_.RegisterOutput(color);
	}

	binaryManager_.Write(saveEditorConfigFileName_);
}
