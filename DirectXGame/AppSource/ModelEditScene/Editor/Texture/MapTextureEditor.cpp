#include "MapTextureEditor.h"
#include <GameCamera/DebugMousePos.h>
#include <Utility/SearchFile.h>

void MapTextureEditor::Initialize(TextureManager* textureManager, Input* input) {
	textureManager_ = textureManager;
	input_ = input;

	TextureLoad();
	//更新処理中のテクスチャロードができないためすぐセーブ
	TextureSave();

	MapLoad();

	size_t stringLength = textureIndexList_[currentTextureIndex_].second.length() + 1;
	std::memcpy(inputText_, textureIndexList_[currentTextureIndex_].second.c_str(), stringLength);
}

void MapTextureEditor::Update() {
	saveTextureIndices_.resize(height_, std::vector<ChipData>(width_, { 0, Direction::Front }));
	textureIndices_.resize(height_, std::vector<ChipData>(width_, { textureIndexList_[0].first, Direction::Front }));

	//回転処理
	if (input_->GetKeyState(DIK_Q) && !input_->GetPreKeyState(DIK_Q)) {
		int nextDir = (static_cast<int>(currentDirection_) + 3) % 4;
		currentDirection_ = static_cast<Direction>(nextDir);
	}

	if (input_->GetKeyState(DIK_E) && !input_->GetPreKeyState(DIK_E)) {
		int nextDir = (static_cast<int>(currentDirection_) + 1) % 4;
		currentDirection_ = static_cast<Direction>(nextDir);
	}

	//テクスチャの割り当て
	if (input_->GetMouseButtonState()[0] && DebugMousePos::isHovered) {
		std::pair<int, int> gridded = {
			static_cast<int>(cursorPos_.x),
			static_cast<int>(cursorPos_.y)
		};

		if (textureIndices_.size() <= gridded.second || textureIndices_[0].size() <= gridded.first) {
			return;
		}

		int textureIndex = textureIndexList_[currentTextureIndex_].first;
		textureIndices_[gridded.second][gridded.first] = { textureIndex, currentDirection_ };
		saveTextureIndices_[gridded.second][gridded.first] = { currentTextureIndex_, currentDirection_ };
	}
}

void MapTextureEditor::DrawImGui() {
#ifdef USE_IMGUI

	ImGui::Begin("MapTexture");

	ImGui::BeginChild("TextureList", ImVec2(600, 0));
	for (int i = 0; i < static_cast<int>(textureIndexList_.size()); ++i) {
		int textureIndex = textureIndexList_[i].first;

		//errorテクスチャは表示しない
		if (textureIndex == textureManager_->GetErrorTextureHandle()) {
			continue;
		}

		ImGui::PushID(i);
		TextureData* textureData = textureManager_->GetTextureData(textureIndex);

		ImVec2 size = ImVec2(50, 50);
		if (currentTextureIndex_ == i) {
			size = ImVec2(60, 60);
		}
		if (ImGui::ImageButton("", ImTextureRef(textureData->GetGPUHandle().ptr), size)) {
			currentTextureIndex_ = i;
			currentDirection_ = Direction::Front;
			std::memcpy(inputText_, textureIndexList_[i].second.c_str(), textureIndexList_[i].second.length() + 1);
		}

		ImGui::PopID();

		if (i % 8 != 5 && i < int(textureIndexList_.size() - 1)) {
			ImGui::SameLine();
		}
	}

	ImGui::EndChild();

	ImGui::SameLine();

	ImGui::BeginChild("Preview", ImVec2(192, 0));
	{
		ImDrawList* dl = ImGui::GetWindowDrawList();

		ImVec2 p = ImGui::GetCursorScreenPos();
		ImVec2 size = { 192, 192 };

		ImVec2 pos[4] = {
			{p.x, p.y},
			{p.x + size.x, p.y},
			{p.x + size.x, p.y + size.y},
			{p.x, p.y + size.y}
		};

		ImVec2 uv[4];
		switch (currentDirection_) {
		case Direction::Front:
			uv[0] = { 0.0f, 0.0f };
			uv[1] = { 1.0f, 0.0f };
			uv[2] = { 1.0f, 1.0f };
			uv[3] = { 0.0f, 1.0f };
			break;
		case Direction::Right:
			uv[0] = { 0.0f, 1.0f };
			uv[1] = { 0.0f, 0.0f };
			uv[2] = { 1.0f, 0.0f };
			uv[3] = { 1.0f, 1.0f };
			break;
		case Direction::Back:
			uv[0] = { 1.0f, 1.0f };
			uv[1] = { 0.0f, 1.0f };
			uv[2] = { 0.0f, 0.0f };
			uv[3] = { 1.0f, 0.0f };
			break;
		case Direction::Left:
			uv[0] = { 1.0f, 0.0f };
			uv[1] = { 1.0f, 1.0f };
			uv[2] = { 0.0f, 1.0f };
			uv[3] = { 0.0f, 0.0f };
			break;
		}

		TextureData* textureData = textureManager_->GetTextureData(textureIndexList_[currentTextureIndex_].first);

		dl->AddImageQuad(
			ImTextureRef(textureData->GetGPUHandle().ptr),
			pos[0], pos[1], pos[2], pos[3],
			uv[0], uv[1], uv[2], uv[3]
		);
	}

	ImGui::EndChild();

	ImGui::SameLine();

	ImGui::BeginChild("FilePathEdit", ImVec2(0, 0));

	ImGui::InputText("|", inputText_, sizeof(inputText_));

	if (ImGui::Button("Edit")) {

	}

	ImGui::EndChild();

	ImGui::End();

	ImGui::Begin("Debug");
	ImGui::Text("Current Direction : %d", static_cast<int>(currentDirection_));
	ImGui::End();

#endif // USE_IMGUI
}

void MapTextureEditor::TextureLoad() {
	std::vector<std::pair<int, std::string>> allTextureIndices;
	auto files = SearchFiles("Assets/Texture/MapChip/", ".png");

	//現在読み込み可能な全テクスチャを読み込み
	for (const auto& filePath : files) {
		int handle = textureManager_->LoadTexture("MapChip/" + filePath);
		allTextureIndices.push_back({ handle, filePath });
	}

	auto values = binaryManager_.Read(saveFileNameTexturePath_);

	//すでに読み込んでいる奴を除外しつつ登録
	for (int i = 0; i < static_cast<int>(values.size()); ++i) {
		std::string texturePath = BinaryManager::Reverse<std::string>(values[i].get());

		//削除は想定していないのでstd::vectorに直接push_back
		int handle = textureManager_->LoadTexture("MapChip/" + texturePath);
		textureIndexList_.push_back({ handle, texturePath });

		//除外処理
		for (size_t i = 0; i < allTextureIndices.size(); ++i) {
			if (allTextureIndices[i].second == texturePath) {
				allTextureIndices.erase(allTextureIndices.begin() + i);
				break;
			}
		}
	}

	//残りを追加
	for (const auto& textureData : allTextureIndices) {
		textureIndexList_.push_back(textureData);
	}

	if (textureIndexList_.empty()) {
		//最低1つはテクスチャが必要なのでwhite1x1をMapchipフォルダにコピーして追加
		std::string white1x1 = "Assets/.EngineResource/Texture/white1x1.png";
		std::string target = "Assets/Texture/MapChip/white1x1.png";
		std::filesystem::copy_file(white1x1, target, std::filesystem::copy_options::overwrite_existing);
		int handle = textureManager_->LoadTexture("MapChip/white1x1.png");
		textureIndexList_.push_back({ handle, "white1x1.png" });
	}
}

void MapTextureEditor::TextureSave() {
	for (int i = 0; i < static_cast<int>(textureIndexList_.size()); ++i) {
		binaryManager_.RegisterOutput(textureIndexList_[i].second);
	}
	binaryManager_.Write(saveFileNameTexturePath_);
}

void MapTextureEditor::MapLoad() {

}

void MapTextureEditor::MapSave() {
}
