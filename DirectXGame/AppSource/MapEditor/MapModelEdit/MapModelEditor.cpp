#include "MapModelEditor.h"

void MapModelEditor::Initialize(ModelManager* modelManager) {
	modelManager_ = modelManager;
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
}

void MapModelEditor::Draw(Window* window) {
	Matrix4x4 vp = camera_->GetVPMatrix();
	renderObject_->CopyBufferData(0, &vp, sizeof(Matrix4x4));
	int textureIndex = currentModel_.materials[0].textureIndex;
	renderObject_->CopyBufferData(1, &textureIndex, sizeof(int));
	renderObject_->Draw(window);
}

void MapModelEditor::DrawImGui() {
#ifdef USE_IMGUI

	ImGui::Begin("MapModelEditor");

	//選択する物の変更
	ImGui::Text("Tile Type: %s", tileTypeStr_[selectedTileType_]);
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
		selectedModelIndex_ = std::min(int(modelPaths_[selectedTileType_].size() - 1), selectedModelIndex_ + 1);
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete")) {
		if (modelPaths_[selectedTileType_].size() > 0) {
			modelPaths_[selectedTileType_].erase(modelPaths_[selectedTileType_].begin() + selectedModelIndex_);
			selectedModelIndex_ = 0;
		}
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
		}
	}

	if (errorMsg_ != "") {
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "%s", errorMsg_.c_str());
	}

	ImGui::End();

#endif
}
