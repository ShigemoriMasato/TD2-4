#pragma once
#include <Game/MapData/Data/MapData.h>
#include <Utility/Vector.h>
#include <Input/Input.h>
#include <map>

class MapTypeEditor {
public:

	void Initialize(MapChipData* mcData, Input* input);
	void Update();
	void DrawImGui(std::map<TileType, Vector3> colorMap);

	void SetCursorPos(const Vector2& pos) { cursorPos_ = pos; }
	bool IsAnySelected() const { return someSelected_; }
	void OtherSelected() { someSelected_ = false; }

	std::pair<int, int> GetGridPos() const {
		return {
			static_cast<int>(width_),
			static_cast<int>(height_)
		};
	}

private:

	MapChipData* mcData_{};
	Vector2 cursorPos_{};
	Input* input_{ nullptr };

	//マップサイズ
	int width_ = 0;
	int height_ = 0;

	std::map<TileType, std::string> tileStrings_{};
	TileType currentType_{ TileType::Air };

	bool someSelected_ = true;
};
