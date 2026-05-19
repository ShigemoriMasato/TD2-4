#include "WeaponImGui.h"
#include <Utility/SearchFile.h>

WeaponImGui::WeaponImGui(SHEngine::Engine* engine) : engine_(engine) {
	auto allFiles = SearchFiles("Assets/Binary/Weapons/", ".bin");

	BinaryManager binManager;

	for (const auto& fileName : allFiles) {
		auto& data = files_[fileName];

		data.modelFilePath = binManager.Reverse<std::string>();
		data.regularMatrix = binManager.Reverse<Matrix4x4>();
		int keyCount = binManager.Reverse<int>();
		for (int i = 0; i < keyCount; ++i) {
			Weapon::Key key;
			key.time = binManager.Reverse<float>();
			int matrixCount = binManager.Reverse<int>();
			for (int j = 0; j < matrixCount; ++j) {
				key.matrices.push_back(binManager.Reverse<Matrix4x4>());
			}
			data.attackAnimation.push_back(key);
		}
	}

	RefleshModelPaths();
}

void WeaponImGui::Update() {
#ifdef USE_IMGUI

	ImGui::Begin("Weapon Edit");

	if (ImGui::TreeNode("File Select")) {

		{
			static char fileName[256] = "";
			ImGui::InputText("新規ファイル名", fileName, sizeof(fileName));
			if (ImGui::Button("Add")) {
				if (files_.find(fileName) != files_.end()) {
					auto& popup = popupMessages_.emplace_back();
					popup.title = "Error";
					popup.message = "そのファイルは既に存在しています";
				} else {
					files_[fileName] = {};
					currentFilePath_ = fileName;
				}

				fileName[0] = '\0';
			}
		}

		if (!files_.empty()) {
			static int currentItem = 0;
			std::vector<const char*> fileNames;
			fileNames.reserve(files_.size());
			for (const auto& [fileName, _] : files_) {
				fileNames.push_back(fileName.c_str());
			}
			ImGui::ListBox("Select File", &currentItem, fileNames.data(), int(fileNames.size()), 4);

			if (ImGui::Button("Delete")) {
				files_.erase(currentFilePath_);
				currentFilePath_ = "";
			}

			currentItem = std::clamp(currentItem, 0, int(files_.size()) - 1);
			currentFilePath_ = fileNames[currentItem];
		}

		ImGui::TreePop();
	}

	if (currentFilePath_.empty()) {
		ImGui::End();
		return;
	}

	if (ImGui::TreeNode("Model")) {
		ImGui::Text("Assets/Models/Item/Weapon/にモデルファイルを入れるとここに表示されます");
		if (!modelPaths_.empty()) {
			std::vector<const char*> modelFileNames;
			modelFileNames.reserve(modelPaths_.size());
			for (const auto& modelPath : modelPaths_) {
				modelFileNames.push_back(modelPath.c_str());
			}

			int currentItem = 0;
			std::string& currentPath = files_[currentFilePath_].modelFilePath;
			for (int i = 0; i < int(modelFileNames.size()); ++i) {
				if (modelFileNames[i] == currentPath) {
					currentItem = i;
					break;
				}
			}

			ImGui::ListBox("Select Model", &currentItem, modelFileNames.data(), int(modelFileNames.size()), 4);

			files_[currentFilePath_].modelFilePath = modelFileNames[currentItem];
		}

		ImGui::TreePop();
	}

	ImGui::End();


	for (int i = 0; i < int(popupMessages_.size()); ++i) {
		ImGui::SetNextWindowFocus();
		ImGui::SetNextWindowSize(ImVec2(300, 0));
		ImGui::SetNextWindowPos({ 640 - i * 20.0f, 360 - i * 20.0f }, ImGuiCond_Always);
		ImGui::Begin(popupMessages_[i].title.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Text("%s", popupMessages_[i].message.c_str());

		if (ImGui::Button("OK")) {
			popupMessages_.erase(popupMessages_.begin() + i);
			--i;
		}

		ImGui::End();
	}

#endif
}

Weapon::Data WeaponImGui::GetData() const {
	if (files_.find(currentFilePath_) == files_.end()) {
		return {};
	}
	return files_.at(currentFilePath_);
}

void WeaponImGui::RefleshModelPaths() {
	modelPaths_.clear();
	auto allFiles = SearchDirectoryNames("Assets/Models/Item/Weapon/");
	for (const auto& fileName : allFiles) {
		modelPaths_.push_back(fileName);
	}
}
