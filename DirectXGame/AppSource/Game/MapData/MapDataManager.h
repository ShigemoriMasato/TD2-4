#pragma once
#include "Data/MapData.h"
#include <Assets/Texture/TextureManager.h>

//外部ファイルとの橋渡し役
class MapDataManager {
public:

	~MapDataManager();

	void Initialize();

	MapDataForBin* GetMapData(const std::string& modelFilePath);
	MapDataForBin* GetMapData(int mapID);

	int GetMapTextureOffset(int mapID);

private:

	void Save();
	void Load();

	std::vector<MapDataForBin> mapData_{};
	std::vector<TextureData*> mapTextures_{};

	std::unique_ptr<BinaryManager> binaryManager_ = nullptr;
	const std::string saveFile_ = "MapData";

	Logger logger_ = nullptr;
};
