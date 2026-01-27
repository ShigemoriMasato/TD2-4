#include"TimeTracker.h"

TimeTracker::TimeTracker() {
	isRunning_ = false;
}

void TimeTracker::StartMeasureTimes() {
	startTime_ = std::chrono::high_resolution_clock::now();
	isRunning_ = true;
}

void TimeTracker::EndMeasureTimes() {
	// 最後の時間
	endTime_ = std::chrono::high_resolution_clock::now();

	isRunning_ = false;

	// クリア時間を取得する
	duration_ = endTime_ - startTime_;
	TimeLimit::totalSeconds = static_cast<int>(duration_.count());
	// 時間から各桁の番号を取得
	TimeLimit::CreateNumbers();
}

void TimeTracker::Update() {
	if (isRunning_) {
		// 現在の時間を取得
		auto currentTime = std::chrono::high_resolution_clock::now();

		// 経過時間を計算 (現在 - 開始)
		duration_ = currentTime - startTime_;

		// TimeLimitのstatic変数を更新
		TimeLimit::totalSeconds = static_cast<int>(duration_.count());
		TimeLimit::CreateNumbers();
	}
}