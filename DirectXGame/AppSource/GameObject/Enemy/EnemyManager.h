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
	void Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager);

	/// @brief EnemyManagerを更新
	/// @param deltaTime デルタタイム（秒）
	/// @param currentWave 現在のWave番号
	/// @param playerPosition プレイヤーの位置
	/// @param vpMatrix VP行列
	void Update(float deltaTime, uint32_t currentWave, const Vector3& playerPosition, const Matrix4x4& vpMatrix);

	/// @brief 敵を描画
	/// @param cmdObj コマンドオブジェクト
	void Draw(CmdObj* cmdObj);

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

	/// @brief マップの境界を設定
	/// @param minX X方向の最小値
	/// @param maxX X方向の最大値
	/// @param minZ Z方向の最小値
	/// @param maxZ Z方向の最大値
	void SetMapBounds(float minX, float maxX, float minZ, float maxZ);

	/// @brief 生存している敵の数を取得
	/// @return 生存している敵の数
	int GetAliveEnemyCount() const;

	/// @brief 敵を削除
	/// @param target 削除する敵
	void RemoveEnemy(Enemy* target);

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

	/// @brief 座標をマップ境界内にクランプする
	/// @param position 座標
	/// @return クランプされた座標
	Vector3 ClampToMapBounds(const Vector3& position) const;

private:

	// 敵のリスト
	std::vector<std::unique_ptr<Enemy>> enemies_;

	// モデルマネージャー
	SHEngine::ModelManager* modelManager_ = nullptr;

	// 描画データマネージャー
	SHEngine::DrawDataManager* drawDataManager_ = nullptr;

	// ターゲット位置（Playerの位置など）
	Vector3 targetPosition_ = { 0.0f, 0.0f, 0.0f };

	// 生成範囲
	float spawnRangeMin_ = 20.0f;
	float spawnRangeMax_ = 30.0f;

	// マップの境界
	float mapMinX_ = -20.0f;
	float mapMaxX_ = 20.0f;
	float mapMinZ_ = -20.0f;
	float mapMaxZ_ = 20.0f;

	// Wave1の自動生成タイマー
	float wave1SpawnTimer_ = 0.0f;
	const float wave1SpawnInterval_ = 2.0f; // 2秒ごとに生成

	// 乱数生成器
	std::random_device rd_;
	std::mt19937 gen_;
	std::uniform_real_distribution<float> angleDist_;
	std::uniform_real_distribution<float> rangeDist_;
	std::uniform_real_distribution<float> concentrationDist_;

};
