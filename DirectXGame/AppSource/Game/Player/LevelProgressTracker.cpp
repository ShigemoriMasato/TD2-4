#include "LevelProgressTracker.h"

void LevelProgressTracker::StartWave(int currentLevel) {
	startLevel_ = currentLevel; // 現在のレベル
	endLevel_ = currentLevel;   // 終了時のレベル
	levelUpsThisWave_ = 0;      // ウェーブ中に上がった量
}

void LevelProgressTracker::OnLevelUp(int newLevel) {
	endLevel_ = newLevel; // 終了時のレベル更新
	levelUpsThisWave_++;  // 1増加
}
