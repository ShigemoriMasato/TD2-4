#pragma once
#include <string>
#include "NewMap.h"
#include <map>

class NewMapManager {
public:

	void Initialize();

	//TileType、描画時のTextureIDとDirectionのセット(現在mapIDでしか判別してない)
	NewMap GetMapData(int stageNum) { return newMapData_[stageNum]; };

	NewMap GetStageMap(int stage, int map);
	//テクスチャIDからパスへの対応表
	std::map<int, std::string> GetTextureMap() { return textureMap_; };

private:

	void LoadTexturePaths();
	void LoadMapData();

	std::vector<NewMap> newMapData_{};

	std::map<int, std::string> textureMap_{};

	const std::string saveFileNameTexturePath_ = "TexturePaths";
	const std::string saveFileNameTextureMap_ = "MapTextureData";
	const std::string saveFileNameMapData_ = "MapChips";
	const std::string saveFileNameStageData_ = "StageMapData";
	BinaryManager binaryManager_;
};
