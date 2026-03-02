#pragma once
#include "Enemy.h"
#include <vector>
#include <memory>
#include <random>

/// @brief 敵を管理するクラス
class EnemyManager {
public:

	EnemyManager() = default;
	~EnemyManager() = default;

	/// @brief EnemyManagerを初期化
	/// @param modelManager モデルマネージャー
	/// @param drawDataManager 描画データマネージャー
	void Initialize();

	/// @brief EnemyManagerを更新
	/// @param deltaTime デルタタイム（秒）
	void Update(float deltaTime);

	/// @brief 敵を描画
	/// @param cmdObj コマンドオブジェクト
	void Draw();

	/// @brief ImGuiでデバッグ情報を表示
	void DrawImGui();

	/// @brief 敵を生成
	/// @param type 敵の種類
	/// @param spawnType 生成の種類
	/// @param count 生成する数
	/// @param currentWave 現在のWave数
	void Generate(EnemyType type, SpawnType spawnType, int count, uint32_t currentWave);

	/// @brief ターゲット位置を設定
	/// @param target ターゲット位置
	void SetTarget(const Vector3& target) { targetPosition_ = target; }

	/// @brief 生存している敵の数を取得
	/// @return 生存している敵の数
	int GetAliveEnemyCount() const;

	/// @brief 全ての敵をクリア
	void Clear();

	/// @brief 敵のリストを取得
	/// @return 敵のリスト
	const std::vector<std::unique_ptr<Enemy>>& GetEnemies() const { return enemies_; }

private:

	/// @brief 生成位置を計算（Scatter用）
	/// @param currentWave 現在のWave数
	/// @return 生成位置
	Vector3 CalculateScatterPosition(uint32_t currentWave);

	/// @brief 生成位置を計算（Concentration用）
	/// @param basePosition 基準位置
	/// @param index インデックス
	/// @param count 生成数
	/// @return 生成位置
	Vector3 CalculateConcentrationPosition(const Vector3& basePosition, int index, int count);

private:

	// 敵のリスト
	std::vector<std::unique_ptr<Enemy>> enemies_;

	// ターゲット位置（Playerの位置など）
	Vector3 targetPosition_ = { 0.0f, 0.0f, 0.0f };

	// 生成範囲
	float spawnRangeMin_ = 20.0f;
	float spawnRangeMax_ = 30.0f;

	// 乱数生成器
	std::random_device rd_;
	std::mt19937 gen_;
	std::uniform_real_distribution<float> angleDist_;
	std::uniform_real_distribution<float> rangeDist_;
	std::uniform_real_distribution<float> concentrationDist_;

};
