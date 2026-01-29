#pragma once
#include <vector>

struct MapConfig {
	int mapID = 0;
	int norma = 20;
};

struct StageData {
	int initOreNum = 10;
	std::vector<MapConfig> configs = {};
};

class StageEditor {
public:

	void SetMapNum(int mapNum) { maxMapNum_ = mapNum; }

	/// @brief ImGuiの描画
	/// @return 画面に描画してほしいマップナンバー。(-1なら変更なし)
	int DrawImGui();

	void Save();

private:

	std::vector<StageData> stageData_ = std::vector<StageData>(1);

	int currentStage_ = 0;
	int currentMapNum_ = 0;
	int maxMapNum_ = 0;
};
