#pragma once
#include <SHEngine.h>
#include <Common/GameWindow.h>
#include <Common/KeyConfig/KeyManager.h>
#include <Game/MapData/MapDataManager.h>
#include <Game/NewMap/NewMapManager.h>

struct CommonData {
	std::unique_ptr<DualDisplay> display;
	std::unique_ptr<GameWindow> mainWindow;
	std::unique_ptr<KeyManager> keyManager;
	//旧(以降したら消します)
	std::unique_ptr<MapDataManager> mapDataManager;
	//新しいやつ。こっちつかってくんろ
	std::unique_ptr<NewMapManager> newMapManager;
	bool exeFinished_ = false;

	int postEffectDrawDataIndex = -1;
};
