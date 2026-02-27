#pragma once
#include <functional>
#include <imgui/imgui.h>

namespace Player{
class LevelSystem {
public:
	// 初期化関数
	void Initialize(int startLevel = 1);

	// 経験値の増加
	void AddExp(float amount);

	// Getter
	int GetLevel() const { return level_; }
	float GetCurrentExp() const { return currentExp_; }
	float GetNextExp() const { return nextExp_; }

	// ImGuiの描画
	void DrawImGui();

	// イベント
	std::function<void(int newLevel)> onLevelUp;
	std::function<void(float currentExp, float nextExp)> onExpChanged;

private:
	int level_ = 1;           // レベル
	float currentExp_ = 0.0f; // 現在の経験値
	float nextExp_ = 10.0f;   // レベルアップに必要な経験値

	// レベルアップに必要な経験値を求める
	void UpdateNextExp();
};
}