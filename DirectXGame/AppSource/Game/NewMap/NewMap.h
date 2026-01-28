#pragma once
#include <Game/MapData/Data/MapData.h>
#include <ModelEditScene/Editor/Texture/MapTextureEditor.h>

struct NewMap {
	//判定まわり
	std::vector<std::vector<TileType>> mapChipData;
	//描画用(地面のデータたち)
	std::vector<std::vector<ChipData>> renderData;
	//Decoration追加予定


	//任意の方向に回転させる
	void Rotate(Direction dir);
};
