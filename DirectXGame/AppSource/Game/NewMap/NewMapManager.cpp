#include "NewMapManager.h"

void NewMapManager::Initialize() {
	LoadTexturePaths();
	LoadMapData();
}

void NewMapManager::LoadTexturePaths() {
	auto values = binaryManager_.Read(saveFileNameTexturePath_);
	if (values.empty()) {
		return;
	}
	int index = 0;

	while (index < values.size()) {
		int textureID = BinaryManager::Reverse<int>(values[index++].get());
		std::string texturePath = BinaryManager::Reverse<std::string>(values[index++].get());
		textureMap_[textureID] = texturePath;
	}
}

void NewMapManager::LoadMapData() {
	auto mapValues = binaryManager_.Read(saveFileNameMapData_);
	auto textureValues = binaryManager_.Read(saveFileNameTextureMap_);
	if (mapValues.empty() || textureValues.empty()) {
		return;
	}

	int mapIndex = 0;
	int textureIndex = 0;

	while(mapIndex < mapValues.size() && textureIndex < textureValues.size()) {
		NewMap newMap;

		//マップの大きさの読み込み
		int width = BinaryManager::Reverse<int>(mapValues[mapIndex++].get());
		int height = BinaryManager::Reverse<int>(mapValues[mapIndex++].get());

		int texWidth = BinaryManager::Reverse<int>(textureValues[textureIndex++].get());
		int texHeight = BinaryManager::Reverse<int>(textureValues[textureIndex++].get());

		//大きさが合わなかったら読み込み失敗
		if(texWidth != width || texHeight != height) {
			return;
		}

		//MapChipデータ読み込み
		newMap.mapChipData.resize(height, std::vector<TileType>(width));
		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				int tileTypeInt = BinaryManager::Reverse<int>(mapValues[mapIndex++].get());
				newMap.mapChipData[y][x] = static_cast<TileType>(tileTypeInt);
			}
		}

		//描画データ読み込み
		newMap.renderData.resize(height, std::vector<ChipData>(width));
		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				int textureID = BinaryManager::Reverse<int>(textureValues[textureIndex++].get());
				int directionInt = BinaryManager::Reverse<int>(textureValues[textureIndex++].get());
				newMap.renderData[y][x] = { textureID, static_cast<Direction>(directionInt) };
			}
		}

		newMapData_.push_back(newMap);
	}
}
