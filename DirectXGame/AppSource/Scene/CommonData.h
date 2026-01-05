#pragma once
#include <SHEngine.h>
#include <Common/GameWindow.h>
#include <Common/KeyConfig/KeyManager.h>

struct CommonData {
	std::unique_ptr<DualDisplay> display;
	std::unique_ptr<GameWindow> mainWindow;
	std::unique_ptr<KeyManager> keyManager;
	bool exeFinished_ = false;

	int postEffectDrawDataIndex = -1;
};
