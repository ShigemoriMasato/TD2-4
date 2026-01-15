#pragma once
#include "Data/MapData.h"
#include "Data/StageData.h"
#include <Assets/Texture/TextureManager.h>

//外部ファイルとの橋渡し役
class MapDataManager {
public:

	~MapDataManager();

	void Initialize();

	//ゲームで使う形に成形したMapDataを取得
	MapData GetMapData(int stageID);

	MapDataForBin* GetRawMapData(const std::string& modelFilePath);
	MapDataForBin* GetRawMapData(int mapID);
	const std::vector<MapDataForBin>& GetAllRawMapData() const { return mapData_; }

	int GetMapTextureOffset(int mapID);

private:

	void Save();
	void Load();

	std::vector<StageData> stageData_{};

	std::vector<MapDataForBin> mapData_{};
	std::vector<TextureData*> mapTextures_{};

	std::unique_ptr<BinaryManager> binaryManager_ = nullptr;
	const std::string mapSaveFile_ = "MapData";
	const std::string stageSaveFile_ = "StageData";

	Logger logger_ = nullptr;
};
