#include "GameTimer.h"

void GameTimer::Initialize() {
	timer_ = 0.0f;
}

void GameTimer::Update(float deltaTime) {
	timer_ += deltaTime;
}

bool GameTimer::IsEnd() const {
	constexpr float kEndTime = 10.0f; // タイマーの終了時間
	return kEndTime < timer_;
}
