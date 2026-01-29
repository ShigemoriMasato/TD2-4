#pragma once
#include <SHEngine.h>
#include <Common/GameWindow.h>
#include <Common/KeyConfig/KeyManager.h>
#include <Game/NewMap/NewMapManager.h>

struct CommonData {
	std::unique_ptr<DualDisplay> display;
	std::unique_ptr<GameWindow> mainWindow;
	std::unique_ptr<KeyManager> keyManager;
	std::unique_ptr<NewMapManager> newMapManager;
	bool exeFinished_ = false;

	int postEffectDrawDataIndex = -1;
};
