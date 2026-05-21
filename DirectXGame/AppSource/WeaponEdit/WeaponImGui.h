#pragma once
#include "Weapon.h"
#include <SHEngine.h>

class WeaponImGui {
public:

	WeaponImGui(SHEngine::Engine* engine);
	~WeaponImGui();

	void Update();
	Weapon::Data GetData() const;

	bool IsDataChanged() const { return isDataChanged_; }

private:

	void RefreshModelPaths();

	void Save();
	void Load();

	struct PopupMessage {
		std::string title;
		std::string message;
	};
	std::vector<PopupMessage> popupMessages_;

	SHEngine::Engine* engine_;

	bool isDataChanged_ = false;

	//ファイルたち
	std::unordered_map<std::string, Weapon::Data> files_;
	std::vector<std::string> modelPaths_;

	std::string currentFilePath_;

	const std::string modelPath_ = "Assets/Model/";
	std::string weaponModelPath_ = "Item/Weapon/";

	const std::string itemSavePath_ = "Weapons/";
	const std::string extension_ = ".bin";
	const std::string saveFile_ = "WeapoonEditorData.bin";
};
