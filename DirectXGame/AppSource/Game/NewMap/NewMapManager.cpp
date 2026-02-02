#include "NewMapManager.h"

void NewMapManager::Initialize(ModelManager* modelManager) {
	modelManager_ = modelManager;
	LoadTexturePaths();
	LoadMapData();
	LoadModelList();
	LoadDecoData();
	LoadStageConfig();
}

CurrentStageConfig NewMapManager::GetStageMap(int stage, int map) {
	CurrentStageConfig config;
	if (stageData_.size() <= stage || stageData_[stage].configs.size() <= map) {
		config.initOreNum = 10;
		config.norma = 10000000;
		config.currentMap = newMapData_[0];
		config.currentMapID = -1;
		return config;
	}
	map = std::clamp(map, 0, int(stageData_[stage].configs.size()));
	int mapID = stageData_[stage].configs[map].mapID;

	config.initOreNum = stageData_[stage].initOreNum;
	config.norma = stageData_[stage].configs[map].norma;
	config.currentMapID = mapID;
	config.currentMap = newMapData_[mapID];

	return config;
}

CurrentStageConfig NewMapManager::GetEndlessMap(int stageCount, int prevMap) {
	std::vector<int> maps;
	if (stageCount < 2) {
		for (int i = 0; i < int(newMapData_.size()); ++i) {
			if (newMapData_[i].endlessPriority == 0 && i != prevMap) {
				maps.push_back(i);
			}
		}
	} else if (stageCount < 5) {
		for (int i = 0; i < int(newMapData_.size()); ++i) {
			if (newMapData_[i].endlessPriority <= 1 && i != prevMap) {
				maps.push_back(i);
			}
		}
	} else {
		for (int i = 0; i < int(newMapData_.size()); ++i) {
			if (newMapData_[i].endlessPriority > 0 && i != prevMap) {
				maps.push_back(i);
			}
		}
	}

	int factID = rand() % int(maps.size());
	CurrentStageConfig config;

	//ノルマの計算（シグモイド関数によるS字カーブ）		- AI作成 -
	// - minNorma: 最小値（stageCount=0付近）
	// - maxNorma: 理論上の最大値（実際は * 0.6f で60%に制限）
	// - midPoint: S字カーブの中心点（ここで加速度が最大になる）
	// - steepness: カーブの急さ（大きいほど急、小さいほど緩やか）
	// 
	// 現在の挙動:
	//   stageCount 0〜3: 緩やかに上昇（+1〜2）
	//   stageCount 4〜9: 加速して上昇（+3〜4）
	//   stageCount 10〜: 再び緩やかに（+1〜0）
	//   stageCount 20時点で約50〜53
	//
	// 調整方法:
	//   - 全体を緩やかに → steepness を小さく（例: 0.3f）
	//   - 全体を急に → steepness を大きく（例: 0.7f）
	//   - 加速開始を遅らせる → midPoint を大きく（例: 10.0f）
	//   - 最終値を上げる → 0.6f を大きく（例: 0.8f）
	float minNorma = 15.0f;
	float maxNorma = 100.0f;
	float midPoint = 7.0f;   // 加速の中心を4と10の間に
	float steepness = 0.5f;  // 急な上昇
	float t = 1.0f / (1.0f + std::exp(-steepness * (stageCount - midPoint)));
	config.norma = int(minNorma + (maxNorma - minNorma) * t * 0.6f);  // 20で約50になるよう調整
	config.initOreNum = 10;
	config.currentMapID = maps[factID];
	config.currentMap = newMapData_[maps[factID]];
	return config;
}

int NewMapManager::GetWallIndex() {
	return modelMap_[0].first;
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

	while (mapIndex < mapValues.size() && textureIndex < textureValues.size()) {
		NewMap newMap;

		//マップの大きさの読み込み
		int width = BinaryManager::Reverse<int>(mapValues[mapIndex++].get());
		int height = BinaryManager::Reverse<int>(mapValues[mapIndex++].get());

		int texWidth = BinaryManager::Reverse<int>(textureValues[textureIndex++].get());
		int texHeight = BinaryManager::Reverse<int>(textureValues[textureIndex++].get());

		//大きさが合わなかったら読み込み失敗
		if (texWidth != width || texHeight != height) {
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

		int endlessPriority = BinaryManager::Reverse<int>(mapValues[mapIndex++].get());
		newMap.endlessPriority = endlessPriority;

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

void NewMapManager::LoadDecoData() {
	auto values = binaryManager_.Read(saveFileNameDecorationData_);

	if (values.empty()) {
		return;
	}

	int index = 0;

	int mapCount = BinaryManager::Reverse<int>(values[index++].get());
	newMapData_.resize(mapCount);

	for (int i = 0; i < mapCount; ++i) { //マップの数だけfor

		int decorationCount = BinaryManager::Reverse<int>(values[index++].get());

		for (int j = 0; j < decorationCount; ++j) {// モデルの数だけfor

			int modelID = BinaryManager::Reverse<int>(values[index++].get());
			int modelIndex = modelMap_[modelID].first;
			int transformCount = BinaryManager::Reverse<int>(values[index++].get());
			std::vector<Transform> transforms;

			for (int k = 0; k < transformCount; ++k) { //配置された数だけfor
				Transform transform;
				transform.position = BinaryManager::Reverse<Vector3>(values[index++].get());
				transform.rotate = BinaryManager::Reverse<Vector3>(values[index++].get());
				transform.scale = BinaryManager::Reverse<Vector3>(values[index++].get());
				transforms.push_back(transform);
			}

			newMapData_[i].decorations[modelIndex] = transforms;
		}

	}
}

void NewMapManager::LoadModelList() {
	auto values = binaryManager_.Read(saveFileNameModelList_);
	if (values.empty()) {
		return;
	}
	int index = 0;
	while (index < values.size()) {
		int modelID = BinaryManager::Reverse<int>(values[index++].get());
		std::string modelPath = BinaryManager::Reverse<std::string>(values[index++].get());
		int modelIndex = modelManager_->LoadModel(".Deco/" + modelPath);
		modelMap_[modelID] = { modelIndex, modelPath };

		//colorは使わないので読み飛ばし
		++index;
	}
}

void NewMapManager::LoadStageConfig() {
	auto values = binaryManager_.Read(saveFileNameStageData_);
	if (values.empty()) {
		return;
	}
	int index = 0;

	int version = BinaryManager::Reverse<int>(values[index++].get());

	if (version == 1) {
		while (index < values.size()) {
			StageData& stage = stageData_.emplace_back();
			int initOre = BinaryManager::Reverse<int>(values[index++].get());
			int mapSize = BinaryManager::Reverse<int>(values[index++].get());
			std::vector<MapConfig> configs;
			configs.resize(mapSize);
			for (int i = 0; i < mapSize; ++i) {
				int mapID = BinaryManager::Reverse<int>(values[index++].get());
				int norma = BinaryManager::Reverse<int>(values[index++].get());
				configs[i] = { mapID, norma };
			}

			stage.initOreNum = initOre;
			stage.configs = configs;
		}
	}
}
