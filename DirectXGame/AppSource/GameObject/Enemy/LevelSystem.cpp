#include "LevelSystem.h"
#include <imgui/imgui.h>

LevelSystem::~LevelSystem() {
	if(isSave_) {
		Save();
	}
}

void LevelSystem::Initialize(EnemyManager* enemyManager, int stageNum, Vector3* playerPosPtr, float minX, float maxX, float minZ, float maxZ) {
	enemyManager_ = enemyManager;
	stageNum_ = stageNum;
	std::string fileName = "Stage" + std::to_string(stageNum_) + ".bytes";
	timer_ = 0.0f;
	isActive_ = true;

	Load();

	// 座標範囲の設定
	minX_ = minX;
	maxX_ = maxX;
	minZ_ = minZ;
	maxZ_ = maxZ;

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
			std::uniform_real_distribution<float> distX(minX_, maxX_);
			std::uniform_real_distribution<float> distZ(minZ_, maxZ_);
			Vector3 spawnPos = { distX(rng_), 0.0f, distZ(rng_) };
			enemyManager_->PopEnemy(spawnPos, static_cast<int>(config_.enemyHp));
		}
	}
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

void LevelSystem::AdjustDifficult() {
	if (allTimer_ <= 10.0f) {

	} else if (allTimer_ <= 20.0f) {
		float levelTimer = allTimer_ - 10.0f;


	}
}

void LevelSystem::Load() {
}

void LevelSystem::Save() {
}
