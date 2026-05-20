#include "WeaponImGui.h"
#include <Utility/SearchFile.h>

WeaponImGui::WeaponImGui(SHEngine::Engine* engine) : engine_(engine) {
	auto allFiles = SearchFiles("Assets/Binary/Weapons/", ".bin");

	BinaryManager binManager;

	Load();
	RefreshModelPaths();
}

WeaponImGui::~WeaponImGui() {
	Save();
}

void WeaponImGui::Update() {
#ifdef USE_IMGUI

	if (ImGui::Begin("File Select")) {

		{
			static char fileName[256] = "";
			ImGui::InputText("新規ファイル名", fileName, sizeof(fileName));
			if (ImGui::Button("Add")) {
				if (files_.find(fileName) != files_.end()) {
					auto& popup = popupMessages_.emplace_back();
					popup.title = "Error";
					popup.message = "そのファイルは既に存在しています";
				} else if(fileName[0] != '\0') {
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

		ImGui::End();
	}

	if (currentFilePath_.empty()) {
		ImGui::End();
		return;
	}

	if (ImGui::Begin("Model")) {
		//モデルを探すパスの変更
		static char currentModelPath[256];
		std::memcpy(currentModelPath, weaponModelPath_.c_str(), 256);
		ImGui::InputText("Search", currentModelPath, 256);
		weaponModelPath_ = currentModelPath;

		if (ImGui::Button("Refresh")) {
			RefreshModelPaths();
		}

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

		ImGui::End();
	}
	
	if (ImGui::Begin("IdleMatrix")) {

		

		ImGui::End();
	}

	/*************   PopUp Message   *************/

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

void WeaponImGui::RefreshModelPaths() {
	modelPaths_.clear();
	auto allFiles = SearchDirectoryNames(modelPath_ + weaponModelPath_);
	for (const auto& fileName : allFiles) {
		modelPaths_.push_back(fileName);
	}
}

void WeaponImGui::Save() {
	BinaryManager binManager;

	int fileCount = int(files_.size());
	binManager.Register(&fileCount);

	for (const auto& [filePath, data] : files_) {
		std::string path = itemSavePath_ + filePath + extension_;
		binManager.Register(&path);

		BinaryManager itemBin;
		itemBin.Register(&data.modelFilePath);
		itemBin.Register(&data.regularMatrix);
		int keyCount = int(data.attackAnimation.size());
		itemBin.Register(&keyCount);
		for (const auto& key : data.attackAnimation) {
			itemBin.Register(&key.time);
			int matrixCount = int(key.matrices.size());
			itemBin.Register(&matrixCount);
			for (const auto& matrix : key.matrices) {
				itemBin.Register(&matrix);
			}
		}
		itemBin.Write(itemSavePath_ + filePath + extension_);
	}

	binManager.Write(saveFile_);
}

void WeaponImGui::Load() {
	BinaryManager binManager;
	BinaryManager itemBin;

	if (!binManager.Boot(saveFile_)) {
		return;
	}

	int fileCount = binManager.Reverse<int>();

	for (int i = 0; i < fileCount; ++i) {
		std::string filePath = binManager.Reverse<std::string>();
		if (!itemBin.Boot(filePath)) {
			continue;
		}
		Weapon::Data data;
		data.modelFilePath = itemBin.Reverse<std::string>();
		data.regularMatrix = itemBin.Reverse<Matrix4x4>();
		int keyCount = itemBin.Reverse<int>();
		for (int j = 0; j < keyCount; ++j) {
			Weapon::Key key;
			key.time = itemBin.Reverse<float>();
			int matrixCount = itemBin.Reverse<int>();
			for (int k = 0; k < matrixCount; ++k) {
				key.matrices.push_back(itemBin.Reverse<Matrix4x4>());
			}
			data.attackAnimation.push_back(key);
		}
		files_[filePath] = data;
	}
}
