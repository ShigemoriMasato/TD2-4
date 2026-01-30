#pragma once
#include <string>
#include "NewMap.h"
#include <map>
#include <Assets/Model/ModelManager.h>
#include <ModelEditScene/Editor/Stage/StageEditor.h>

struct CurrentStageConfig {
	int initOreNum = 10;
	int currentMapID = -1;
	int norma = 0;
	NewMap currentMap;
};

class NewMapManager {
public:

	void Initialize(ModelManager* modelManager);

	//TileType、描画時のTextureIDとDirectionのセット(現在mapIDでしか判別してない)
	NewMap GetMapData(int stageNum) { return newMapData_[stageNum]; };

	CurrentStageConfig GetStageMap(int stage, int map);
	//テクスチャIDからパスへの対応表
	std::map<int, std::string> GetTextureMap() { return textureMap_; };

private:

	void LoadTexturePaths();
	void LoadMapData();
	void LoadDecoData();
	void LoadModelList();
	void LoadStageConfig();

	std::vector<NewMap> newMapData_{};

	std::map<int, std::string> textureMap_{};
	//ID -> [Index, path]
	std::map<int, std::pair<int, std::string>> modelMap_{};
	std::vector<StageData> stageData_{};

	const std::string saveFileNameTexturePath_ = "TexturePaths";
	const std::string saveFileNameTextureMap_ = "MapTextureData";
	const std::string saveFileNameMapData_ = "MapChips";
	const std::string saveFileNameStageData_ = "StageConfig";
	const std::string saveFileNameDecorationData_ = "MapDecorationData";
	const std::string saveFileNameModelList_ = "MapDecorationModelList";
	const std::string saveFileName_ = "StageConfig";
	BinaryManager binaryManager_;

	ModelManager* modelManager_ = nullptr;
};
