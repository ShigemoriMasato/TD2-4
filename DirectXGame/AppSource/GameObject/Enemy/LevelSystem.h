#pragma once
#include <GameObject/Enemy/EnemyManager.h>
#include <Utility/DataStructures.h>
#include <Tool/Binary/BinaryManager.h>
#include <random>

class LevelSystem {
public:

	~LevelSystem();

	void Initialize(EnemyManager* enemyManager, int stageNum, Vector3* playerPosPtr, float minX, float maxX, float minZ, float maxZ);

	void Update(float deltaTime);
	void Stop() { isActive_ = false; }

	void DrawImGui();

private:

	void AdjustDifficult();

	void Load();
	void Save();

	enum class Event : uint32_t {
		SpawnNormalEnemy,	// 敵をスポーン
		MiddleBoss,			// 中ボス
		FinalBoss,			// ボス
	};

	struct BaseSystem {
		float spawnInterval = 3.0f; // 敵をスポーンする間隔
		float enemyCount = 2; // スポーンする敵の数
		float enemyHp = 5.0f; // スポーンする敵のHP
		uint32_t event = uint32_t(Event::SpawnNormalEnemy); // イベント
	};

	BaseSystem config_;

	EnemyManager* enemyManager_ = nullptr;

	int stageNum_ = 0;
	float timer_ = 0.0f;
	float allTimer_ = 0.0f;

	std::mt19937 rng_{ std::random_device{}() };

	float minX_ = -20;
	float maxX_ = 20;
	float minZ_ = -20;
	float maxZ_ = 20;

	bool isActive_ = false;

private:	//Edit用

	bool isSave_ = true;

	float increaseIntercal_ = 0.1f; // スポーン間隔の増加量
	float increaseEnemyCount_ = 0.5f; // スポーンする敵の数の増加量
	float increaseHp_ = 5.0f; // 敵のHPの増加量

	BinaryManager binaryManager_;

	Vector3* playerPosPtr_ = nullptr;
};
