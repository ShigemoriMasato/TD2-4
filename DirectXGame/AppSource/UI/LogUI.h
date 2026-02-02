#pragma once
#include"Object/SpriteObject.h"

class LogUI {
public:

	void Initialize(DrawData drawData,int confTex,int effectTex);

	void Update();

	void Draw(Window* window, const Matrix4x4& vpMatrix);

	// 衝突ログを出す
	void AddUnitConflictLog();

private:

	struct LogData {
		std::unique_ptr<SpriteObject> spriteObject_;
		std::unique_ptr<SpriteObject> effectObject_;

		bool isActive_ = false;
		float timer_ = 0.0f;
	};

private:

	std::vector<LogData> logDatas_;

	float maxTime_ = 2.0f;
};
