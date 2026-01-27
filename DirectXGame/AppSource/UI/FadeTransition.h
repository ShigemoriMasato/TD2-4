#pragma once
#include"Object/SpriteObject.h"

class FadeTransition {
public:
	enum class Phase {
		In,
		Out
	};


	void Initialize(DrawData drawData);

	void Update();

	void Draw(Window* window, const Matrix4x4& vpMatrix);

	void SetFade(Phase phase) {
		if (isAnimation_) { return; }
		phase_ = phase;
		timer_ = 0.0f;
		isAnimation_ = true;
	}

public:

	// アニメーション中華を判断
	bool IsAnimation() const { return isAnimation_; }

	// 終了の判断
	bool IsFinished() const { return isFinished_; }

private:

	// 背景画像
	std::unique_ptr<SpriteObject> bgSpriteObject_;

	// アニメーション中か判断
	bool isAnimation_ = false;
	// 終了を判断
	bool isFinished_ = false;

	Phase phase_ = Phase::In;

	float timer_ = 0.0f;

private: // 調整項目

	float inTime_ = 1.0f;
	float outTime_ = 1.0f;
};