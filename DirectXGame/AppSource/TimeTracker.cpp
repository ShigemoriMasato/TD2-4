#include"TimeTracker.h"

TimeTracker::TimeTracker() {
	isRunning_ = false;
	accumulatedTime_ = std::chrono::duration<float>::zero();
	duration_ = std::chrono::duration<float>::zero();
}

void TimeTracker::StartMeasureTimes() {
	if (!isRunning_) {
		startTime_ = std::chrono::high_resolution_clock::now();
		isRunning_ = true;
	}
}

void TimeTracker::EndMeasureTimes() {
	if (isRunning_) {
		// 最後の時間
		endTime_ = std::chrono::high_resolution_clock::now();

		// 今回の計測区間（End - Start）を蓄積時間に加算する
		accumulatedTime_ += (endTime_ - startTime_);

		isRunning_ = false;

		// 最終的な合計時間を更新
		duration_ = accumulatedTime_;

		TimeLimit::totalSeconds = static_cast<int>(duration_.count());
		// 時間から各桁の番号を取得
		TimeLimit::CreateNumbers();
	}
}

void TimeTracker::Reset() {
	// 計測を停止し、時間を0に戻す
	isRunning_ = false;
	accumulatedTime_ = std::chrono::duration<float>::zero();
	duration_ = std::chrono::duration<float>::zero();

	// 表示も0に更新
	TimeLimit::totalSeconds = 0;
	TimeLimit::CreateNumbers();
}

void TimeTracker::Update() {
	if (isRunning_) {
		// 現在の時間を取得
		auto currentTime = std::chrono::high_resolution_clock::now();

		// 経過時間を計算： 「これまでの蓄積時間」 + 「今回の区間の経過時間(現在 - 開始)」
		duration_ = accumulatedTime_ + (currentTime - startTime_);

		// TimeLimitのstatic変数を更新
		TimeLimit::totalSeconds = static_cast<int>(duration_.count());
		TimeLimit::CreateNumbers();
	}
}