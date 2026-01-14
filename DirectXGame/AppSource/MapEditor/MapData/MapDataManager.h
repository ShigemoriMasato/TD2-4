#pragma once
#include "Data/MapData.h"
#include <Assets/Texture/TextureManager.h>

//外部ファイルとの橋渡し役
class MapDataManager {
public:

	~MapDataManager();

	void Initialize();
	
	MapData* GetMapData(const std::string& modelFilePath);
	MapData* GetMapData(int mapID);

	int GetMapTextureOffset(int mapID);

private:

	void Save();
	void Load();

	std::vector<MapData> mapData_;
	std::vector<TextureData*> mapTextures_;

	std::unique_ptr<BinaryManager> binaryManager_ = nullptr;
	const std::string saveFile_ = "MapData";
};
