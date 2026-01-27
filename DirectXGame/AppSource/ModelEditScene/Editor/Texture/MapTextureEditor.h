#pragma once
#include <Assets/Texture/TextureManager.h>
#include <Input/Input.h>
#include <Game/MapData/Data/MapData.h>

struct ChipData {
	int textureIndex;
	Direction direction;
};

class MapTextureEditor {
public:

	void Initialize(TextureManager* textureManager, Input* input);
	void Update();
	void DrawImGui();

	void SetMapSize(int width, int height) { width_ = width; height_ = height; }
	void SetCursorPos(const Vector2& pos) { cursorPos_ = pos; }

	void NonEdit() { editing_ = false; }
	bool isSomeSelected() const { return someSelected_; }
	void AnySelect() { someSelected_ = false; }

	void SetCurrentStage(int currentStage);

	std::vector<std::vector<ChipData>>& GetTextureIndices() { return fieldData_; }

	void TextureSave();
	void MapSave();

private:

	void TextureLoad();
	void MapLoad();

	//CurrentStageのデータ。indexを保存
	std::vector<std::vector<ChipData>> fieldData_{};
	//ステージ全体像。idを保存
	std::vector<std::vector<std::vector<ChipData>>> saveData_{};

	int currentStage_;
	int currentTextureID_ = 0;
	Direction currentDirection_ = Direction::Front;

	std::map<int, std::string> textureIndexList_{};

	int width_ = 0;
	int height_ = 0;
	Vector2 cursorPos_{};

	char inputText_[256] = {};

	bool someSelected_ = false;
	bool editing_ = false;

	Input* input_;
	TextureManager* textureManager_;

	BinaryManager binaryManager_;
	const std::string saveFileNameTexturePath_ = "TexturePaths";
	const std::string saveFileNameMapTexture_ = "MapTextureData";
};
