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
	allTimer_ = 0.0f;
	waveCount_ = 0;
	isActive_ = true;

	Load();

	// 座標範囲の設定
	mapInfo_ = mapInfo;

	playerPosPtr_ = playerPosPtr;

	// 敵ごとのベースステータスを初期化        // HP // Attack
	baseStatusMap_[EnemyType::Normal] = { 3.0f, 1.0f };
	baseStatusMap_[EnemyType::Fast]   = { 1.0f, 1.0f };
	baseStatusMap_[EnemyType::Tackle] = { 5.0f, 2.0f };
}

void LevelSystem::Update(float deltaTime) {
	timer_ += deltaTime;
	allTimer_ += deltaTime;

	waveCount_ = static_cast<int>(allTimer_ / 30.0f);

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
			
			// 敵の種類ごとにWave上昇時のボーナススケールを変更
			float hpScale = 0.0f;
			float attackScale = 0.0f;

			if (type == EnemyType::Normal) {
				hpScale = 2.0f;
				attackScale = 0.5f;
			} else if (type == EnemyType::Fast) {
				hpScale = 1.0f;
				attackScale = 0.25f;
			} else if (type == EnemyType::Tackle) {
				hpScale = 3.0f;
				attackScale = 1.0f;
			}

			// ベースステータスの取得とWaveボーナスの適用
			float baseHp = baseStatusMap_[type].hp;
			float baseAttack = baseStatusMap_[type].attack;

			float finalHp = baseHp + waveCount_ * hpScale;
			float finalAttack = baseAttack + waveCount_ * attackScale;

			enemyManager_->PopEnemy(spawnPos, static_cast<int>(finalHp), type, finalAttack);
		}
	}
}

std::vector<std::vector<EnemyType>> LevelSystem::GetNext5WaveTypes() const {
	std::vector<std::vector<EnemyType>> waves;
	
	int currentWave = waveCount_;
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

	if (ImGui::TreeNode("Enemy Base Status")) {
		if (ImGui::TreeNode("Normal")) {
			ImGui::DragFloat("HP", &baseStatusMap_[EnemyType::Normal].hp, 0.1f, 1.0f, 1000.0f);
			ImGui::DragFloat("Attack", &baseStatusMap_[EnemyType::Normal].attack, 0.1f, 1.0f, 1000.0f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Fast")) {
			ImGui::DragFloat("HP", &baseStatusMap_[EnemyType::Fast].hp, 0.1f, 1.0f, 1000.0f);
			ImGui::DragFloat("Attack", &baseStatusMap_[EnemyType::Fast].attack, 0.1f, 1.0f, 1000.0f);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Tackle")) {
			ImGui::DragFloat("HP", &baseStatusMap_[EnemyType::Tackle].hp, 0.1f, 1.0f, 1000.0f);
			ImGui::DragFloat("Attack", &baseStatusMap_[EnemyType::Tackle].attack, 0.1f, 1.0f, 1000.0f);
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}

	ImGui::End();

#endif
}

std::vector<EnemyType> LevelSystem::GetSpawnTypesAtTime(float time) const {
	if (time <= 30.0f) {
		return { EnemyType::Normal };
		//return { EnemyType::Tackle };
	} else if (time <= 60.0f) {
		return { EnemyType::Fast };
		//return { EnemyType::Tackle };
	} else if (time <= 90.0f) {
		return { EnemyType::Normal, EnemyType::Fast };
	} else if (time <= 120.0f) {
		return { EnemyType::Tackle };
	} else if (time <= 150.0f) {
		return { EnemyType::Normal, EnemyType::Tackle };
	} else if (time <= 180.0f) {
		return { EnemyType::Fast, EnemyType::Tackle };
	} else if (time <= 210.0f) {
		return { EnemyType::Normal, EnemyType::Fast, EnemyType::Tackle };
	} else if (time <= 240.0f) {
		return { EnemyType::Normal, EnemyType::Fast, EnemyType::Tackle };
	} else if (time <= 270.0f) {
		return { EnemyType::Normal, EnemyType::Fast, EnemyType::Tackle };
	} else if (time <= 300.0f) {
		return { EnemyType::Normal, EnemyType::Fast, EnemyType::Tackle };
	} else {
		// Wave 11以降はランダム
		return { EnemyType::Normal, EnemyType::Fast, EnemyType::Tackle };
	}
}

void LevelSystem::AdjustDifficult() {
	// Wave 1 (0-30秒)
	if (allTimer_ <= 30.0f) {
		config_.spawnInterval = 2.5f;
		config_.enemyCount = 1.0f;
	}
	// Wave 2 (30-60秒)
	else if (allTimer_ <= 60.0f) {
		config_.spawnInterval = 3.0f;
		config_.enemyCount = 2.0f;
	}
	// Wave 3 (60-90秒)
	else if (allTimer_ <= 90.0f) {
		config_.spawnInterval = 2.5f;
		config_.enemyCount = 1.0f;
	}
	// Wave 4 (90-120秒)
	else if (allTimer_ <= 120.0f) {
		config_.spawnInterval = 3.0f;
		config_.enemyCount = 2.0f;
	}
	// Wave 5 (120-150秒)
	else if (allTimer_ <= 150.0f) {
		config_.spawnInterval = 2.5f;
		config_.enemyCount = 2.0f;
	}
	// Wave 6 (150-180秒)
	else if (allTimer_ <= 180.0f) {
		config_.spawnInterval = 2.5f;
		config_.enemyCount = 1.0f;
	}
	// Wave 7 (180-210秒)
	else if (allTimer_ <= 210.0f) {
		config_.spawnInterval = 2.5f;
		config_.enemyCount = 2.0f;
	}
	// Wave 8 (210-240秒)
	else if (allTimer_ <= 240.0f) {
		config_.spawnInterval = 2.5f;
		config_.enemyCount = 2.0f;
	}
	// Wave 9 (240-270秒)
	else if (allTimer_ <= 270.0f) {
		config_.spawnInterval = 2.0f;
		config_.enemyCount = 3.0f;
	}
	// Wave 10 (270-300秒)
	else if (allTimer_ <= 300.0f) {
		config_.spawnInterval = 2.0f;
		config_.enemyCount = 3.0f;
	}
	// Wave 11以降 (300秒以降) - ランダムパターン
	else {
		config_.spawnInterval = 1.8f;
		config_.enemyCount = 3.0f;
	}

	config_.spawnTypes = GetSpawnTypesAtTime(allTimer_);
}

void LevelSystem::Load() {
}

void LevelSystem::Save() {
}
