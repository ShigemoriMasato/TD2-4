#include "MapDataManager.h"

void MapDataManager::Initialize() {
	logger_ = getLogger("Game");
	Load();
}

MapDataManager::~MapDataManager() {
	Save();
}

MapDataForBin* MapDataManager::GetMapData(const std::string& modelFilePath) {
	for (auto& map : mapData_) {
		if (map.modelFilePath == modelFilePath) {
			return &map;
		}
	}
	return nullptr;
}

MapDataForBin* MapDataManager::GetMapData(int mapID) {
	for (auto& map : mapData_) {
		if (map.mapID == mapID) {
			return &map;
		}
	}
	return nullptr;
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
		binaryManager_->RegisterOutput(map.modelFilePath);
		binaryManager_->RegisterOutput(map.width);
		binaryManager_->RegisterOutput(map.height);
		binaryManager_->RegisterOutput(map.mapID);
		binaryManager_->RegisterOutput(int(map.tileData.size()));
		for (const auto& tile : map.tileData) {
			binaryManager_->RegisterOutput(tile);
		}
	}

	binaryManager_->Write(saveFile_);
}

void MapDataManager::Load() {
	auto values = binaryManager_->Read(saveFile_);
	if (values.empty()) {
		return;
	}
	size_t index = 0;
	int mapCount = BinaryManager::Reverse<int>(values[index++].get());
	mapData_.resize(mapCount);
	int nextID = 0;
	for (int i = 0; i < mapCount; ++i) {
		mapData_[i].modelFilePath = BinaryManager::Reverse<std::string>(values[index++].get());
		mapData_[i].width = BinaryManager::Reverse<int>(values[index++].get());
		mapData_[i].height = BinaryManager::Reverse<int>(values[index++].get());
		mapData_[i].mapID = BinaryManager::Reverse<int>(values[index++].get());

		int tileDataSize = BinaryManager::Reverse<int>(values[index++].get());
		mapData_[i].tileData.reserve(10000);
		for (int j = 0; j < tileDataSize; ++j) {
			mapData_[i].tileData.push_back((TileType)BinaryManager::Reverse<int>(values[index++].get()));
		}

		mapData_[i].Verify();
	}
}
