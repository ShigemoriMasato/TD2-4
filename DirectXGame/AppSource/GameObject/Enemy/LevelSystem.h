#pragma once
#include <GameObject/Enemy/EnemyManager.h>
#include <Utility/DataStructures.h>
#include <Tool/Binary/BinaryManager.h>
#include <random>
#include <GameObject/Map/MapInfo.h>

struct WaveVertex {
	float intensity;
	float time;
};

class LevelSystem {
public:

	~LevelSystem();

	void Initialize(EnemyManager* enemyManager, Vector3* playerPosPtr, const MapInfo& mapInfo);
	void Update(float deltaTime);
	void DrawImGui();

	std::vector<WaveVertex> GetWaveVertices() const { return waveVertices_; }

	//設定されたWave時間を超過したかどうか
	bool End() { return end_; }

private:

	void Load();
	void Save();

	void Sort();

	std::mt19937 rng_{ std::random_device{}() };

	MapInfo mapInfo_;
	Vector3* playerPosPtr_ = nullptr;

	std::vector<WaveVertex> waveVertices_;
	int currentWaveIndex_ = 0;
	EnemyManager* enemyManager_ = nullptr;

	float timer_ = 0.0f;


	bool end_ = false;

private:	//Edit用

	BinaryManager binaryManager_;

	bool stop_ = false;
	int editWaveIndex_ = 0;

};
