#pragma once
#include"Object/SpriteObject.h"
#include <list>

class LogUI {
public:

	void Initialize(DrawData drawData,int confTex,int effectTex,int deathTex);

	void Update();

	void Draw(Window* window, const Matrix4x4& vpMatrix);

	// 衝突ログを出す
	void AddUnitConflictLog();

	// ユニットの死亡ログを出す
	void AddUnitDeathLog();

private:

	struct LogData {
		std::unique_ptr<SpriteObject> spriteObject_;
		std::unique_ptr<SpriteObject> effectObject_;

		bool isActive_ = false;
		float timer_ = 0.0f;

		bool inAnimation = false;
	};

private:

	std::vector<LogData> logDatas_;

	// ログの古い順番を確認する
	std::list<LogData*> activeLogList_;

	float maxTime_ = 1.2f;

	int conflTex_ = 0;
	int deathTex_ = 0;
};
