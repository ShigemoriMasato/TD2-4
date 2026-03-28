#include "LevelSystem.h"
#include <imgui/imgui.h>

LevelSystem::~LevelSystem() {
	if(isSave_) {
		Save();
	}
}

void LevelSystem::Initialize(EnemyManager* enemyManager, int stageNum, Vector3* playerPosPtr, const MapInfo& mapInfo) {
	enemyManager_ = enemyManager;
	stageNum_ = stageNum;
	std::string fileName = "Stage" + std::to_string(stageNum_) + ".bytes";
	timer_ = 0.0f;
	isActive_ = true;

	Load();

	// 座標範囲の設定
	mapInfo_ = mapInfo;

	playerPosPtr_ = playerPosPtr;
}

void LevelSystem::Update(float deltaTime) {
	timer_ += deltaTime;
	allTimer_ += deltaTime;

	//レベルデザインだお
	AdjustDifficult();

	if(timer_ >= config_.spawnInterval) {
		timer_ = 0.0f;

		for(int i = 0; i < static_cast<int>(config_.enemyCount); ++i) {
			std::uniform_real_distribution<float> distX(mapInfo_.minX, mapInfo_.maxX);
			std::uniform_real_distribution<float> distZ(mapInfo_.minZ, mapInfo_.maxZ);
			Vector3 spawnPos;
			
			// プレイヤーから半径5m以上離れた位置にスポーンさせる
			const float minSpawnDistanceSq = 5.0f * 5.0f;
			int retryCount = 0;
			do {
				spawnPos = { distX(rng_), 0.0f, distZ(rng_) };
				
				if (playerPosPtr_) {
					float dx = spawnPos.x - playerPosPtr_->x;
					float dz = spawnPos.z - playerPosPtr_->z;
					float distSq = dx * dx + dz * dz;
					if (distSq >= minSpawnDistanceSq) {
						break;
					}
				} else {
					break;
				}
				retryCount++;
			} while (retryCount < 10); // 無限ループ防止のため最大10回リトライ

			EnemyType type = EnemyType::Normal;
			if (!config_.spawnTypes.empty()) {
				int randIndex = rand() % config_.spawnTypes.size();
				type = config_.spawnTypes[randIndex];
			}
			
			enemyManager_->PopEnemy(spawnPos, static_cast<int>(config_.enemyHp), type);
		}
	}
}

std::vector<std::vector<EnemyType>> LevelSystem::GetNext5WaveTypes() const {
	std::vector<std::vector<EnemyType>> waves;
	
	int currentWave = static_cast<int>(allTimer_ / 30.0f);
	for (int i = 0; i < 5; ++i) {
		int targetWave = currentWave + i;
		float timeForWave = targetWave * 30.0f + 15.0f; // mid of that wave
		waves.push_back(GetSpawnTypesAtTime(timeForWave));
	}
	
	return waves;
}

void LevelSystem::DrawImGui() {
#ifdef USE_IMGUI

	ImGui::Begin("Level System");

	ImGui::DragFloat("Spawn Interval", &config_.spawnInterval, 0.1f, 0.1f, 10.0f);
	ImGui::DragFloat("Enemy Count", &config_.enemyCount, 0.1f, 0.1f, 100.0f);
	ImGui::DragFloat("Enemy HP", &config_.enemyHp, 0.1f, 1.0f, 1000.0f);

	ImGui::End();

#endif
}

std::vector<EnemyType> LevelSystem::GetSpawnTypesAtTime(float time) const {
	if (time <= 30.0f) {
		return { EnemyType::Normal };
	} else if (time <= 60.0f) {
		return { EnemyType::Normal };
	} else if (time <= 90.0f) {
		return { EnemyType::Fast };
	} else if (time <= 120.0f) {
		return { EnemyType::Normal, EnemyType::Fast };
	} else {
		return { EnemyType::Normal, EnemyType::Fast };
	}
}

void LevelSystem::AdjustDifficult() {
	// ゲーム開始から2分(120秒)まで10秒ごとのレベルデザイン

	if (allTimer_ <= 30.0f) {

		config_.spawnInterval = 2.5f;
		config_.enemyCount = 1.0f;
		config_.enemyHp = 5.0f;
		
		config_.spawnTypes = { EnemyType::Normal };

	} else if (allTimer_ <= 60.0f) {

		config_.spawnInterval = 3.0f;
		config_.enemyCount = 2.0f;
		config_.enemyHp = 7.0f;

		config_.spawnTypes    = { EnemyType::Normal };

	} else if (allTimer_ <= 90.0f) {

		config_.spawnInterval = 2.5f;
		config_.enemyCount = 1.0f;
		config_.enemyHp = 3.0f;
		
		config_.spawnTypes = { EnemyType::Fast };

	} else if (allTimer_ <= 120.0f) {

		config_.spawnInterval = 3.0f;
		config_.enemyCount = 2.0f;
		config_.enemyHp = 5.0f;

		config_.spawnTypes = { EnemyType::Normal, EnemyType::Fast };

	} else {

		config_.spawnInterval = 2.0f;
		config_.enemyCount = 2.0f;
		config_.enemyHp = 10.0f;

	}

	config_.spawnTypes = GetSpawnTypesAtTime(allTimer_);
}

void LevelSystem::Load() {
}

void LevelSystem::Save() {
}
