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
	bool requireExeFinished = false;
	bool exeFinished = true;

	int nextStageIndex = -1;
	uint32_t nextMapIndex = 0;
	//EndlessMode用
	int stageCount = 0;
	//EndlessMode用
	int prevMapIndex = -1;
	bool isEndlessMode = false;

	//ゲームシーンの設定
	int norma = 100;
	int goldNum = 200;
	int oreNum = 100;

	// 次のシーンのおれユニットの数
	int32_t nextOreUnitMaxNum = 20;

	int postEffectDrawDataIndex = -1;

	// タイトルシーンを受けたか判断する
	bool isTitle_ = false;

	std::vector<int> maxGoldNum_{};
	int currentGoldNum_{};
};
