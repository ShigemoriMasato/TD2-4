#include "MapData.h"

bool MapData::Verify() const {
	return false;
}

void MapData::Load(const std::vector<std::shared_ptr<ValueBase>>& values) {
	if (values.empty()) {
		return;
	}

	int index = 0;
	modelFilePath = BinaryManager::Reverse<std::string>(values[index++].get());
	width = BinaryManager::Reverse<int>(values[index++].get());
	height = BinaryManager::Reverse<int>(values[index++].get());
	mapID = BinaryManager::Reverse<int>(values[index++].get());
	tileData.clear();
	for (int i = 0; i < width * height; ++i) {
		tileData.push_back(BinaryManager::Reverse<int>(values[index++].get()));
	}
}
