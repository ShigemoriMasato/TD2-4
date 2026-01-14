#pragma once
#include "Data/MapData.h"
#include <Assets/Texture/TextureManager.h>

class MapDataManager {
public:

	~MapDataManager();

	void Initialize(TextureManager* textureManager);
	
	MapData* GetMapData(const std::string& modelFilePath);
	MapData* GetMapData(int mapID);

	//描画処理。新しく作成したマップがあればスクリーンショットとしてマップテクスチャを作成する
	void CreateMapTexture();

	int GetMapTextureOffset(int mapID);

private:

	void Save();
	void Load();

	std::vector<MapData> mapData_;
	std::vector<TextureData*> mapTextures_;

	TextureManager* textureManager_ = nullptr;

	std::unique_ptr<BinaryManager> binaryManager_ = nullptr;
	const std::string saveFile_ = "MapData";
};
