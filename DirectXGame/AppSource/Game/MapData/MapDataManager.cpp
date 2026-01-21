#include "MapDataManager.h"

void MapDataManager::Initialize() {
	binaryManager_ = std::make_unique<BinaryManager>();
	logger_ = getLogger("Game");
	Load();
}

MapDataManager::~MapDataManager() {
	Save();
}

StageData MapDataManager::GetStageData(int stageID) {
	for (int i = 0; i < stageData_.size(); ++i) {
		if (stageData_[i].stageID == stageID) {
			return stageData_[i];
		}
	}

	logger_->error("MapDataManager::GetStageData() failed: Stage ID {} not found", stageID);
	return StageData{};
}

MapData MapDataManager::GetMapData(int mapID, Direction direction) {
	MapDataForBin* rawData = GetRawMapData(mapID);
	MapData mapData;
	mapData.mapID = rawData->mapID;

	//方向に応じた幅・高さ・タイル配置を取得
	if (direction == Direction::Front || direction == Direction::Back) {
		mapData.width = rawData->width;
		mapData.height = rawData->height;
	} else {
		mapData.width = rawData->height;
		mapData.height = rawData->width;
	}
	//向きを考慮した二次元配列のtileGridに変換
	mapData.tileGrid = rawData->GetDirectionGrid(direction);

	//整合性チェック
	mapData.Verify();

	return mapData;
}

MapDataForBin* MapDataManager::GetRawMapData(int mapID) {
	for (auto& map : mapData_) {
		if (map.mapID == mapID) {
			return &map;
		}
	}

	MapDataForBin newMap;
	newMap.mapID = mapID;
	newMap.tileData.reserve(10000);
	newMap.tileData.push_back(TileType::Air);
	mapData_.push_back(newMap);
	return &mapData_.back();
}

int MapDataManager::GetMapTextureOffset(int mapID) {
	for (size_t i = 0; i < mapData_.size(); ++i) {
		if (mapData_[i].mapID == mapID) {
			return static_cast<int>(i);
		}
	}
	return -1;
}

void MapDataManager::Save() {
	binaryManager_->RegisterOutput(int(mapData_.size()));

	for (const auto& map : mapData_) {
		binaryManager_->RegisterOutput(map.width);
		binaryManager_->RegisterOutput(map.height);
		binaryManager_->RegisterOutput(map.mapID);
		binaryManager_->RegisterOutput(int(map.tileData.size()));
		for (const auto& tile : map.tileData) {
			binaryManager_->RegisterOutput(int(tile));
		}
	}

	binaryManager_->Write(mapSaveFile_);

	//StageDataの保存権はStageEditorのみに限る
}

void MapDataManager::Load() {
	auto values = binaryManager_->Read(mapSaveFile_);
	size_t index = 0;
	int mapCount = 0;

	if (values.empty()) {
		goto StageDataLoad;
	}

	mapCount = BinaryManager::Reverse<int>(values[index++].get());
	mapData_.resize(mapCount);
	for (int i = 0; i < mapCount; ++i) {
		mapData_[i].width = BinaryManager::Reverse<int>(values[index++].get());
		mapData_[i].height = BinaryManager::Reverse<int>(values[index++].get());
		mapData_[i].mapID = BinaryManager::Reverse<int>(values[index++].get());

		int tileDataSize = BinaryManager::Reverse<int>(values[index++].get());
		if (values.size() < index + tileDataSize) {
			logger_->error("MapDataManager::Load() failed: Incomplete data for map ID {}", mapData_[i].mapID);
			mapData_[i].tileData.resize(tileDataSize);
			break;
		}
		mapData_[i].tileData.reserve(10000);
		for (int j = 0; j < tileDataSize; ++j) {
			mapData_[i].tileData.push_back((TileType)BinaryManager::Reverse<int>(values[index++].get()));
		}

		mapData_[i].Verify();
	}

StageDataLoad:
	values = binaryManager_->Read(stageSaveFile_);
	if (values.empty()) {
		return;
	}
	index = 0;
	int stageCount = BinaryManager::Reverse<int>(values[index++].get());
	stageData_.resize(stageCount);
	for (int i = 0; i < stageCount; ++i) {
		stageData_[i].stageID = BinaryManager::Reverse<int>(values[index++].get());
		stageData_[i].mapID = BinaryManager::Reverse<int>(values[index++].get());
		stageData_[i].direction = (Direction)BinaryManager::Reverse<int>(values[index++].get());
		stageData_[i].goldFrequency = BinaryManager::Reverse<float>(values[index++].get());
	}
}
