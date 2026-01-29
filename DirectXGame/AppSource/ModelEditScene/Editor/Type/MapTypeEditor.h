#pragma once
#include <Game/NewMap/NewMap.h>
#include <Utility/Vector.h>
#include <Input/Input.h>
#include <map>

class MapTypeEditor {
public:

	void Initialize(Input* input);
	void Update();
	void DrawImGui();

	void SetCurrentStage(int currentStage);
	void SetCursorPos(const Vector2& pos) { cursorPos_ = pos; }
	bool isSomeSelected() const { return someSelected_; }
	void OtherSelected() { someSelected_ = false; }

	void NonEdit() { editing_ = false; }

	std::map<TileType, Vector3>& GetColorMap() { return colorMap_; }
	const MapChipData& GetCurrentMapChipData() const { return mcData_[currentStage_]; }

	std::pair<int, int> GetGridPos() const {
		return {
			static_cast<int>(width_),
			static_cast<int>(height_)
		};
	}

	int GetMapNum() const { return static_cast<int>(mcData_.size()); }

	void Save();

private:

	void Load();

	std::vector<MapChipData> mcData_{};
	int currentStage_ = 0;
	Vector2 cursorPos_{};
	Input* input_{ nullptr };

	//マップサイズ
	int width_ = 0;
	int height_ = 0;

	std::map<TileType, std::string> tileStrings_{};
	TileType currentType_{ TileType::Air };

	//ColorMap
	std::map<TileType, Vector3> colorMap_{};

	bool someSelected_ = true;
	bool editing_ = false;

	BinaryManager binaryManager_;

	std::string saveEditorConfigFileName_ = "MapTypeEditorConfig";
	std::string saveStageFileName_ = "MapChips";
};
