#include "MapTextureEditor.h"
#include <GameCamera/DebugMousePos.h>

void MapTextureEditor::Initialize(TextureManager* textureManager, Input* input) {
	textureManager_ = textureManager;
	input_ = input;
}

void MapTextureEditor::Update() {
	if (input_->GetMouseButtonState()[0] && DebugMousePos::isHovered) {
		std::pair<int, int> gridded = {
			static_cast<int>(cursorPos_.x),
			static_cast<int>(cursorPos_.y)
		};

		if (textureIndices_.size() <= gridded.second || textureIndices_[0].size() <= gridded.first) {
			return;
		}

		textureIndices_[gridded.second][gridded.first] = currentTextureIndex_;
	}
}

void MapTextureEditor::DrawImGui() {
#ifdef USE_IMGUI

	ImGui::Begin("MapTexture");

	ImGui::BeginChild("TextureList", ImVec2(600, 0));
	for (int i = 0; i < static_cast<int>(textureIndexList_.size()); ++i) {
		ImGui::PushID(i);
		int textureIndex = textureIndexList_[i];
		TextureData* textureData = textureManager_->GetTextureData(textureIndex);

		ImVec2 size = ImVec2(50, 50);
		if (currentTextureIndex_ == textureIndex) {
			size = ImVec2(60, 60);
		}
		if (ImGui::ImageButton("", ImTextureRef(textureData->GetResource()), size)) {
			currentTextureIndex_ = textureIndex;
			currentDirection_ = Direction::Front;
		}

		ImGui::PopID();

		if (i % 6 != 5 && i < int(textureIndexList_.size() - 1)) {
			ImGui::SameLine();
		}
	}
	if (ImGui::Button("+")) {
		isShowAddText_ = true;
	}

	if (isShowAddText_) {
		ImGui::InputText("AddTexture", inputText_, sizeof(inputText_));
		if (ImGui::Button("Load")) {
			isShowAddText_ = false;
			int handle = textureManager_->LoadTexture(inputText_);
			
			if (handle == 2) {
				errorMsg_ = "Failed to load texture: " + std::string(inputText_);
			} else {
				textureIndexList_.push_back(handle);
				textureFilePaths_[handle] = std::string(inputText_);
			}
		}
	}

	ImGui::EndChild();

	ImGui::SameLine();

	ImGui::BeginChild("Preview", ImVec2(64, 0));
	{
		if(textureIndexList_.empty()){
			ImGui::Text("No Texture");
			ImGui::EndChild();
			ImGui::End();
			return;
		}
		TextureData* textureData = textureManager_->GetTextureData(textureIndexList_[currentTextureIndex_]);
		ImVec2 uv0, uv1;

		switch (currentDirection_) {
		case Direction::Front:
			uv0 = ImVec2(0.0f, 0.0f);
			uv1 = ImVec2(1.0f, 1.0f);
			break;
		case Direction::Right:
			uv0 = ImVec2(0.0f, 1.0f);
			uv1 = ImVec2(1.0f, 0.0f);
			break;
		case Direction::Back:
			uv0 = ImVec2(0.0f, 0.0f);
			uv1 = ImVec2(1.0f, 1.0f);
			break;
		case Direction::Left:
			uv0 = ImVec2(1.0f, 0.0f);
			uv1 = ImVec2(0.0f, 1.0f);
			break;
		}

		ImGui::Image(ImTextureID(textureData->GetGPUHandle().ptr), { 64, 64 }, uv0, uv1);
	}

	if (ImGui::Button("Delete")) {
		for (int i = 0; i < textureIndexList_.size(); ++i) {
			if (textureIndexList_[i] == currentTextureIndex_) {
				//要素の削除
				textureIndexList_.erase(textureIndexList_.begin() + i);
				textureFilePaths_.erase(currentTextureIndex_);

				//現在選択中のテクスチャをエラーテクスチャに変更
				currentTextureIndex_ = textureManager_->GetErrorTextureHandle();
				break;
			}
		}
	}

	ImGui::EndChild();

	ImGui::End();

#endif // USE_IMGUI
}
