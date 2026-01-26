#pragma once
#include"Object/SpriteObject.h"

class GameOverUI {
public:

	void Initialize(DrawData drawData);

	void Update();

	void Draw(Window* window, const Matrix4x4& vpMatrix);

private:

	// 背景画像
	std::unique_ptr<SpriteObject> bgSpriteObject_;

	float timer_ = 0.0f;

	// 背景の移動処理
	float startBgPosY_ = 1080.0f;
	float endBgPosY_ = 360.0f;

	bool inAnimation_ = true;

private: // 調整項目

	float maxTime_ = 1.0f;

};