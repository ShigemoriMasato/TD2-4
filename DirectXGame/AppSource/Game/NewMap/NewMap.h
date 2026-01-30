#pragma once
#include <vector>
#include <map>
#include <Utility/DataStructures.h>
#include <Tool/Binary/BinaryManager.h>

enum class TileType {
	Air,
	Road,
	Wall,
	Home,
	Gold,
	Count
};

enum class Direction {
	Front,
	Right,
	Back,
	Left
};

struct ChipData {
	int textureIndex;
	Direction direction;
};

using MapChipData = std::vector<std::vector<TileType>>;

struct NewMap {
	//判定まわり
	MapChipData mapChipData;
	//描画用(地面のデータたち)
	std::vector<std::vector<ChipData>> renderData;
	//Decoration追加予定
	std::map<int, std::vector<Transform>> decorations;

	//任意の方向に回転させる
	void Rotate(Direction dir);
};
