#pragma once
#include <GameObject/Enemy/EnemyManager.h>
#include <Utility/DataStructures.h>
#include <Tool/Binary/BinaryManager.h>
#include <random>
#include <GameObject/Map/MapInfo.h>

class LevelSystem {
public:

	~LevelSystem();

	void Initialize(EnemyManager* enemyManager, int stageNum, Vector3* playerPosPtr, const MapInfo& mapInfo);

	void Update(float deltaTime);
	void Stop() { isActive_ = false; }

	void DrawImGui();

	std::vector<std::vector<EnemyType>> GetNext5WaveTypes() const;
	int GetCurrentWave() const { return waveCount_; }

private:

	void AdjustDifficult();
	std::vector<EnemyType> GetSpawnTypesAtTime(float time) const;

	void Load();
	void Save();

	struct EnemyBaseStatus {
		float hp = 1.0f;
		float attack = 1.0f;
	};

	struct BaseSystem {
		float spawnInterval = 3.0f; // 敵をスポーンする間隔
		float enemyCount = 2; // スポーンする敵の数
		std::vector<EnemyType> spawnTypes = { EnemyType::Normal }; // スポーンする敵の種類
	};

	BaseSystem config_;
	std::map<EnemyType, EnemyBaseStatus> baseStatusMap_;

	EnemyManager* enemyManager_ = nullptr;

	int stageNum_ = 0;
	float timer_ = 0.0f;
	float allTimer_ = 0.0f;

	std::mt19937 rng_{ std::random_device{}() };

	MapInfo mapInfo_;

	bool isActive_ = false;

private:	//Edit用

	bool isSave_ = true;

	float increaseIntercal_ = 0.1f; // スポーン間隔の増加量
	float increaseEnemyCount_ = 0.5f; // スポーンする敵の数の増加量
	float increaseHp_ = 5.0f; // 敵のHPの増加量

	// 30秒が経過した回数
	int waveCount_ = 0;

	BinaryManager binaryManager_;

	Vector3* playerPosPtr_ = nullptr;
};
