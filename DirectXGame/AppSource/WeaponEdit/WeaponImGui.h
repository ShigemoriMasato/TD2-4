#pragma once
#include "Weapon.h"
#include <SHEngine.h>

class WeaponImGui {
public:

	WeaponImGui(SHEngine::Engine* engine);

	void Update();
	Weapon::Data GetData() const;

private:

	void RefleshModelPaths();

	struct PopupMessage {
		std::string title;
		std::string message;
	};
	std::vector<PopupMessage> popupMessages_;

	SHEngine::Engine* engine_;

	//ファイルたち
	std::unordered_map<std::string, Weapon::Data> files_;
	std::vector<std::string> modelPaths_;

	std::string currentFilePath_;

};
