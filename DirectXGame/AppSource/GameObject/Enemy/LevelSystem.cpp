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
	waveCount_ = 1;
	isActive_ = true;
	lastScheduledWave_ = -1;
	scheduledNormals_.clear();
	scheduledFastGroups_.clear();
	fastSpawnTimer_ = 0.0f;
	scheduledTackles_.clear();
	tackleSpawnTimer_ = 0.0f;

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
	fastSpawnTimer_ += deltaTime;
	tackleSpawnTimer_ += deltaTime;

	int currentWave = static_cast<int>(allTimer_ / 30.0f) + 1;
	
	// Waveが変わったらNormalEnemyのスケジュールを作成
	if (currentWave != lastScheduledWave_) {
		waveCount_ = currentWave;
		lastScheduledWave_ = currentWave;
		
		// このWaveで出現する敵タイプを取得
		std::vector<EnemyType> typesInThisWave = GetSpawnTypesAtTime(allTimer_);
		bool hasNormal = false;
		bool hasFast = false;
		bool hasTackle = false;
		
		for (const auto& type : typesInThisWave) {
			if (type == EnemyType::Normal) hasNormal = true;
			if (type == EnemyType::Fast) hasFast = true;
			if (type == EnemyType::Tackle) hasTackle = true;
		}
		
		// NormalEnemyのスケジュール作成
		if (hasNormal && waveCount_ > 0) {
			// Wave数 × 5体のNormalEnemyを30秒間で等間隔に配置
			int normalCount = waveCount_ * 5;
			float interval = 30.0f / static_cast<float>(normalCount);
			float waveStartTime = static_cast<float>(waveCount_ - 1) * 30.0f;
			
			// ステータス計算
			float hpScale = 2.0f;
			float attackScale = 0.5f;
			float baseHp = baseStatusMap_[EnemyType::Normal].hp;
			float baseAttack = baseStatusMap_[EnemyType::Normal].attack;
			float finalHp = baseHp + waveCount_ * hpScale;
			float finalAttack = baseAttack + waveCount_ * attackScale;
			
			// スケジュール作成
			std::uniform_real_distribution<float> distX(mapInfo_.minX, mapInfo_.maxX);
			std::uniform_real_distribution<float> distZ(mapInfo_.minZ, mapInfo_.maxZ);
			const float minSpawnDistanceSq = 5.0f * 5.0f;
			
			for (int i = 0; i < normalCount; ++i) {
				ScheduledNormalEnemy scheduled;
				scheduled.spawnTime = waveStartTime + interval * static_cast<float>(i);
				scheduled.hp = static_cast<int>(finalHp);
				scheduled.attack = finalAttack;
				
				// スポーン位置を決定（プレイヤーから5m以上離す）
				int retryCount = 0;
				do {
					scheduled.position = { distX(rng_), 0.0f, distZ(rng_) };
					
					if (playerPosPtr_) {
						float dx = scheduled.position.x - playerPosPtr_->x;
						float dz = scheduled.position.z - playerPosPtr_->z;
						float distSq = dx * dx + dz * dz;
						if (distSq >= minSpawnDistanceSq) {
							break;
						}
					} else {
						break;
					}
					retryCount++;
				} while (retryCount < 10);
				
				scheduledNormals_.push_back(scheduled);
			}
		}
		
		// TackleEnemyのスケジュール作成
		if (hasTackle && waveCount_ > 0) {
			// Wave数 × 2体のTackleEnemyを30秒間で等間隔に配置
			int tackleCount = waveCount_ * 2;
			float interval = 30.0f / static_cast<float>(tackleCount);
			float waveStartTime = static_cast<float>(waveCount_ - 1) * 30.0f;
			
			// ステータス計算
			float hpScale = 3.0f;
			float attackScale = 1.0f;
			float baseHp = baseStatusMap_[EnemyType::Tackle].hp;
			float baseAttack = baseStatusMap_[EnemyType::Tackle].attack;
			float finalHp = baseHp + waveCount_ * hpScale;
			float finalAttack = baseAttack + waveCount_ * attackScale;
			
			// スケジュール作成
			std::uniform_real_distribution<float> distX(mapInfo_.minX, mapInfo_.maxX);
			std::uniform_real_distribution<float> distZ(mapInfo_.minZ, mapInfo_.maxZ);
			const float minSpawnDistanceSq = 5.0f * 5.0f;
			
			for (int i = 0; i < tackleCount; ++i) {
				ScheduledTackleEnemy scheduled;
				scheduled.spawnTime = waveStartTime + interval * static_cast<float>(i);
				scheduled.hp = static_cast<int>(finalHp);
				scheduled.attack = finalAttack;
				
				// スポーン位置を決定（プレイヤーから5m以上離す）
				int retryCount = 0;
				do {
					scheduled.position = { distX(rng_), 0.0f, distZ(rng_) };
					
					if (playerPosPtr_) {
						float dx = scheduled.position.x - playerPosPtr_->x;
						float dz = scheduled.position.z - playerPosPtr_->z;
						float distSq = dx * dx + dz * dz;
						if (distSq >= minSpawnDistanceSq) {
							break;
						}
					} else {
						break;
					}
					retryCount++;
				} while (retryCount < 10);
				
				scheduledTackles_.push_back(scheduled);
			}
		}
	} else {
		waveCount_ = currentWave;
	}

	//レベルデザインだお
	AdjustDifficult();
	
	// スケジュールされたNormalEnemyをスポーン
	auto it = scheduledNormals_.begin();
	while (it != scheduledNormals_.end()) {
		if (allTimer_ >= it->spawnTime) {
			enemyManager_->PopEnemy(it->position, it->hp, EnemyType::Normal, it->attack);
			it = scheduledNormals_.erase(it);
		} else {
			++it;
		}
	}
	
	// スケジュールされたTackleEnemyをスポーン
	auto itTackle = scheduledTackles_.begin();
	while (itTackle != scheduledTackles_.end()) {
		if (allTimer_ >= itTackle->spawnTime) {
			enemyManager_->PopEnemy(itTackle->position, itTackle->hp, EnemyType::Tackle, itTackle->attack);
			itTackle = scheduledTackles_.erase(itTackle);
		} else {
			++itTackle;
		}
	}
	
	// FastEnemyの3秒ごとのグループスポーン
	if (fastSpawnTimer_ >= 3.0f) {
		fastSpawnTimer_ = 0.0f;
		
		// このWaveでFastEnemyが出現するか確認
		std::vector<EnemyType> typesInThisWave = GetSpawnTypesAtTime(allTimer_);
		bool hasFast = false;
		for (const auto& type : typesInThisWave) {
			if (type == EnemyType::Fast) {
				hasFast = true;
				break;
			}
		}
		
		if (hasFast && waveCount_ > 0) {
			// Wave数分のFastEnemyをグループで出現させる
			std::uniform_real_distribution<float> distX(mapInfo_.minX, mapInfo_.maxX);
			std::uniform_real_distribution<float> distZ(mapInfo_.minZ, mapInfo_.maxZ);
			const float minSpawnDistanceSq = 5.0f * 5.0f;
			
			Vector3 centerPos;
			int retryCount = 0;
			do {
				centerPos = { distX(rng_), 0.0f, distZ(rng_) };
				
				if (playerPosPtr_) {
					float dx = centerPos.x - playerPosPtr_->x;
					float dz = centerPos.z - playerPosPtr_->z;
					float distSq = dx * dx + dz * dz;
					if (distSq >= minSpawnDistanceSq) {
						break;
					}
				} else {
					break;
				}
				retryCount++;
			} while (retryCount < 10);
			
			// ステータス計算
			float hpScale = 1.0f;
			float attackScale = 0.25f;
			float baseHp = baseStatusMap_[EnemyType::Fast].hp;
			float baseAttack = baseStatusMap_[EnemyType::Fast].attack;
			float finalHp = baseHp + waveCount_ * hpScale;
			float finalAttack = baseAttack + waveCount_ * attackScale;
			
			// Wave数分のFastEnemyをまとめて出現
			for (int i = 0; i < waveCount_; ++i) {
				// 基準位置からランダムなオフセットを加える（半径2.0以内）
				float offsetX = (float(rand() % 200) / 100.0f - 1.0f) * 2.0f; // -2.0 ~ 2.0
				float offsetZ = (float(rand() % 200) / 100.0f - 1.0f) * 2.0f; // -2.0 ~ 2.0
				Vector3 groupSpawnPos = centerPos;
				groupSpawnPos.x += offsetX;
				groupSpawnPos.z += offsetZ;
				
				enemyManager_->PopEnemy(groupSpawnPos, static_cast<int>(finalHp), EnemyType::Fast, finalAttack);
			}
		}
	}
}

std::vector<std::vector<EnemyType>> LevelSystem::GetNext5WaveTypes() const {
	std::vector<std::vector<EnemyType>> waves;
	
	int currentWave = waveCount_;
	for (int i = 0; i < 5; ++i) {
		int targetWave = currentWave + i;
		float timeForWave = (targetWave - 1) * 30.0f + 15.0f; // mid of that wave
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
