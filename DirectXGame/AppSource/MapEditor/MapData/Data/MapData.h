#pragma once
#include <string>
#include <vector>
#include <Tool/Binary/BinaryManager.h>

struct MapData {
	std::string modelFilePath;
	int width = 0;
	int height = 0;
	int mapID = -1;
	std::vector<int> tileData;

	bool Verify() const;
	void Load(const std::vector<std::shared_ptr<ValueBase>>& values);
};
