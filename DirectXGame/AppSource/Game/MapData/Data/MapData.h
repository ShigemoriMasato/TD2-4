#pragma once
#include <string>
#include <vector>
#include <Tool/Binary/BinaryManager.h>

enum class TileType : int {
	Air,
	Road,
	Wall,
	Home,

	Count
};

struct MapDataForBin {
	std::string modelFilePath;
	int width = 1;
	int height = 1;
	int mapID = -1;
	std::vector<TileType> tileData;

	bool Verify() const;
};

struct MapData {
	std::vector<std::vector<TileType>> tileGrid;
	int width = 1;
	int height = 1;
	int mapID = -1;
	std::string modelFilePath;

	bool Verify() const;
};
