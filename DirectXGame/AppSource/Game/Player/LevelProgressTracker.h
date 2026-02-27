#pragma once
class LevelProgressTracker {
public:
	// ウェーブ開始時にメンバ変数を初期化する関数
	void StartWave(int currentLevel);

	// レベルアップ時の処理
	void OnLevelUp(int newLevel);

	// Getter
	int GetLevelUpsThisWave() const { return levelUpsThisWave_; }
	int GetStartLevel() const { return startLevel_; }
	int GetEndLevel() const { return endLevel_; }

private:
	int startLevel_;       // 開始時のレベル
	int endLevel_;         // 終了時のレベル
	int levelUpsThisWave_; // ウェーブ中にどれくらいレベルが上がったかを記録する変数
};
