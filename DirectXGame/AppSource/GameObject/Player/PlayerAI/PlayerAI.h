#pragma once
#include <../Engine/Utility/Vector.h>
#include <GameObject/Enemy/IEnemy.h>
#include <vector>

class PlayerAI {
public:
	// AIのモード
	enum class AIMode {
		AGGRESSIVE, // 敵に突っ込み続ける
		EVASIVE,    // 敵から逃げ続ける
		SKIRMISH    // 逃げながらもちょっかいをかける
	};

	// 敵のリストから移動方向を求める
	Vector3 ComputeMoveDirection(const Vector3& playerPos, const std::vector<IEnemy*>& enemies, float deltaTime);

	// ターゲットの設定
	void SetTargetEnemy(IEnemy* target) { targetEnemy_ = target; }

	// ターゲットの取得
	IEnemy* GetTargetEnemy() const { return targetEnemy_; }

	// ImGui描画
	void DrawImGui();

private:
	// プレイヤーに最も近い敵を求める
	IEnemy* FindNearestEnemy(const Vector3& playerPos, const std::vector<IEnemy*>& enemies);

private:
	// 追跡開始距離
	float chaseRange_ = 15.0f;

	// 逃走開始距離
	float escapeRange_ = 5.0f;

	// 直前のフレームの移動方向
	Vector3 currentDir_ = {0.0f, 0.0f, 0.0f};

	// 現在ターゲットにしている敵
	IEnemy* targetEnemy_ = nullptr;

	// 移動方向を維持するフレーム数
	float moveTimer_ = 0;

	// ロックされた目標方向
	Vector3 lockedTargetDir_ = {0.0f, 0.0f, 0.0f};

	// 現在のAIモード
	AIMode currentMode_ = AIMode::SKIRMISH;

	// タイマー乱数
	float minTime_ = 0.5f;
	float maxTime_ = 1.0f;
};