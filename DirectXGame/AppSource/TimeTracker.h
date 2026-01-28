#pragma once
#include <chrono>
#include"TimeLimit.h"

class TimeTracker {
public:

	TimeTracker();

	// 時間の計測を開始
	void StartMeasureTimes();

	// 時間の計測を終了
	void EndMeasureTimes();

	// 時間を0にリセット
	void Reset();

	// 更新処理
	void Update();

private:

	// 最初の時間
	std::chrono::time_point<std::chrono::high_resolution_clock> startTime_;

	// 最後の時間
	std::chrono::time_point<std::chrono::high_resolution_clock> endTime_;

	// 経過時間を保存
	std::chrono::duration<float> duration_;

	// これまでの蓄積時間を保存
	std::chrono::duration<float> accumulatedTime_;

	// 計測時間の取得
	bool isRunning_ = false;
};