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

	std::vector<std::vector<ChipData>>& GetTextureIndices() { return textureIndices_; }

private:

	void TextureLoad();
	void TextureSave();
	void MapLoad();
	void MapSave();

	//テクスチャインデックス配列(ステージ)
	std::vector<std::vector<ChipData>> textureIndices_{};
	//保存用テクスチャインデックスインデックス配列
	std::vector<std::vector<ChipData>> saveTextureIndices_{};

	//現在選択中のテクスチャインデックスインデックス
	int currentTextureIndex_ = 0;
	Direction currentDirection_{ Direction::Front };

	int width_ = 0;
	int height_ = 0;
	Vector2 cursorPos_{};

	bool isShowAddText_ = false;
	char inputText_[256] = {};

	std::string errorMsg_{};

	Input* input_;
	TextureManager* textureManager_;
	//テクスチャインデックスを確定的なIDとして保持するリスト(保存対象)
	std::vector<std::pair<int, std::string>> textureIndexList_;

	BinaryManager binaryManager_;
	const std::string saveFileNameTexturePath_ = "TexturePaths";
	const std::string saveFileNameMapTexture_ = "MapTextureData";
};
