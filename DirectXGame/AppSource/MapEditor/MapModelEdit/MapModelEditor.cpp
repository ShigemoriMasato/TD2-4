#include "MapModelEditor.h"

void MapModelEditor::Initialize(ModelManager* modelManager, DrawDataManager* drawDataManager) {
	modelManager_ = modelManager;
	drawDataManager_ = drawDataManager;
	camera_ = std::make_unique<DebugCamera>();
	camera_->Initialize();
	camera_->SetCenter({ 0.0f, 0.0f, 0.0f });

	tileTypeStr_ = {
		{TileType::Air, "Air"},
		{TileType::Road, "Road"},
		{TileType::Wall, "Wall"},
		{TileType::Home, "Home"},
		{TileType::Gold, "Gold"},
	};

	renderObject_ = std::make_unique<RenderObject>();
	renderObject_->Initialize();
	renderObject_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "MapModelEditor_MatrixCBV");
	renderObject_->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "MapModelEditor_TextureIndexCBV");
	renderObject_->SetUseTexture(true);
	renderObject_->psoConfig_.vs = "Simple.VS.hlsl";
	renderObject_->psoConfig_.ps = "PostEffect/Simple.PS.hlsl";
}

void MapModelEditor::Update() {
	camera_->Update();
	//モデルの切り替え
	if (!modelPaths_[selectedTileType_].empty()) {
		std::string path = modelPaths_[selectedTileType_][selectedModelIndex_];
		currentModel_ = modelManager_->GetNodeModelData(modelManager_->LoadModel(path));
		auto drawData = drawDataManager_->GetDrawData(currentModel_.drawDataIndex);
		renderObject_->SetDrawData(drawData);
	}
}

void MapModelEditor::Draw(Window* window) {
	if (currentModel_.materials.empty()) {
		return;
	}
	Matrix4x4 vp = camera_->GetVPMatrix();
	renderObject_->CopyBufferData(0, &vp, sizeof(Matrix4x4));
	int textureIndex = currentModel_.materials.back().textureIndex;
	renderObject_->CopyBufferData(1, &textureIndex, sizeof(int));
	renderObject_->Draw(window);
}

void MapModelEditor::DrawImGui() {
#ifdef USE_IMGUI

	ImGui::Begin("MapModelEditor");

	//選択する物の変更
	ImGui::Text("Tile Type: %s", tileTypeStr_[selectedTileType_].c_str());
	if (ImGui::Button("<<")) {
		selectedTileType_ = static_cast<TileType>(std::max(static_cast<int>(selectedTileType_) - 1, 0));
	}
	ImGui::SameLine();
	if (ImGui::Button(">>")) {
		selectedTileType_ = static_cast<TileType>(std::min(static_cast<int>(selectedTileType_) + 1, static_cast<int>(TileType::Count) - 1));
	}

	ImGui::Separator();

	ImGui::Text("Model Index: %d", selectedModelIndex_);
	ImGui::SameLine();
	if (ImGui::Button("-")) {
		selectedModelIndex_ = std::max(selectedModelIndex_ - 1, 0);
	}
	ImGui::SameLine();
	if (ImGui::Button("+")) {
		selectedModelIndex_ = int(std::min(uint32_t(modelPaths_[selectedTileType_].size() - 1), uint32_t(selectedModelIndex_ + 1)));
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete")) {
		if (modelPaths_[selectedTileType_].size() > 0) {
			modelPaths_[selectedTileType_].erase(modelPaths_[selectedTileType_].begin() + selectedModelIndex_);
			selectedModelIndex_ = 0;
		}
	}

	if (!modelPaths_[selectedTileType_].empty()) {
		ImGui::Text("Current Model Path: %s", modelPaths_[selectedTileType_][selectedModelIndex_].c_str());
	}

	//Modelの追加
	ImGui::InputText("ModelPath", willLoadPath_, sizeof(willLoadPath_));
	ImGui::Text("Add a Selecting Type");
	if (ImGui::Button("Add")) {
		int modelID = modelManager_->LoadModel(std::string(willLoadPath_));
		if (modelID == 1) {
			errorMsg_ = "Failed to Load Model!";
		} else {
			errorMsg_ = "";
			modelPaths_[selectedTileType_].push_back(std::string(willLoadPath_));
			willLoadPath_[0] = '\0';
		}
	}

	if (errorMsg_ != "") {
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "%s", errorMsg_.c_str());
	}

	ImGui::End();

#endif
}

void MapModelEditor::Save() {
	for(int i = 0; i < static_cast<int>(TileType::Count); i++) {
		TileType type = static_cast<TileType>(i);
		binManager_.RegisterOutput(static_cast<int>(type));
		binManager_.RegisterOutput(static_cast<int>(modelPaths_[type].size()));
		for (const auto& path : modelPaths_[type]) {
			binManager_.RegisterOutput(path);
		}
	}
	binManager_.Write("MapModelEditorData");
}

void MapModelEditor::Load() {
	auto values = binManager_.Read("MapModelEditorData");
	size_t index = 0;
	for (int i = 0; i < static_cast<int>(TileType::Count); i++) {
		TileType type = static_cast<TileType>(MapModelEditor::binManager_.Reverse<int>(values[index++].get()));
		int pathCount = MapModelEditor::binManager_.Reverse<int>(values[index++].get());
		modelPaths_[type].clear();
		for (int j = 0; j < pathCount; j++) {
			std::string path = MapModelEditor::binManager_.Reverse<std::string>(values[index++].get());
			modelPaths_[type].push_back(path);
		}
	}
}
