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

	int nextStageIndex = -1;
	uint32_t nextMapIndex = 0;
	//EndlessMode用
	int stageCount = 0;
	//EndlessMode用
	int prevMapIndex = -1;
	bool isEndlessMode = false;

	int postEffectDrawDataIndex = -1;
};
