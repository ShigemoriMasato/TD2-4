#pragma once
#include"Object/SpriteObject.h"

class SelectStageUI {
public:

	void Init(DrawData drawData);

	void Update();

	void Draw(Window* window, const Matrix4x4& vpMatrix);

	bool IsAnimation() const { return isAnimation_; }

	void SetAnimation() {
		if (isAnimation_) { return; }
		isAnimation_ = true;
		timer_ = 0.0f;
	}

private:

	// 画像データ
	std::unique_ptr<SpriteObject> spriteObject_;

	// アニメーションのフラグ
	bool isAnimation_ = false;

	Vector3 defaultSize_ = {};

	float timer_ = 0.0f;

	bool isChange_ = false;

private: // 調整項目

	float maxTime_ = 1.0f;
};