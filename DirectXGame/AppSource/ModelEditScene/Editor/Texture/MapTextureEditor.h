#pragma once
#include <Assets/Texture/TextureManager.h>
#include <Input/Input.h>
#include <Game/MapData/Data/MapData.h>

class MapTextureEditor {
public:

	void Initialize(TextureManager* textureManager, Input* input);
	void Update();
	void DrawImGui();

	void SetMapSize(int width, int height) { width_ = width; height_ = height; }
	void SetCursorPos(const Vector2& pos) { cursorPos_ = pos; }

private:

	std::vector<std::vector<int>> textureIndices_{};
	int currentTextureIndex_ = 0;
	Direction currentDirection_{ Direction::Front };
	std::vector<int> textureIndexList_{};

	int width_ = 0;
	int height_ = 0;
	Vector2 cursorPos_{};

	bool isShowAddText_ = false;
	char inputText_[256] = {};

	std::string errorMsg_{};

	Input* input_;
	TextureManager* textureManager_;
	std::map<int, std::string> textureFilePaths_{};
};
