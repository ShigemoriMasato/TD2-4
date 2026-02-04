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

	std::unique_ptr<DualDisplay> miniMapDisplay;

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

	//今までのやつ
	int maxOreNum = 0;
	int killOreNum = 0;
	int getOreNum = 0;
	int maxGoldNum = 0;
	std::vector<std::pair<int, int>> normaAndScore_{};
	std::vector<int> goldNumRanking_{};
	//今までの合計Gold数
	int sumGoldNum_{};

	int postEffectDrawDataIndex = -1;

	// タイトルシーンを受けたか判断する
	bool isTitle_ = false;
};
