#include "EnemyManager.h"
#include <Assets/Model/ModelManager.h>
#include <numbers>
#include <cmath>

#ifdef USE_IMGUI
#include <imgui/imgui.h>
#endif

void EnemyManager::Initialize() {

	// 乱数生成器の初期化
	gen_ = std::mt19937(rd_());
	angleDist_ = std::uniform_real_distribution<float>(0.0f, static_cast<float>(2.0 * std::numbers::pi));
	rangeDist_ = std::uniform_real_distribution<float>(spawnRangeMin_, spawnRangeMax_);
	concentrationDist_ = std::uniform_real_distribution<float>(-2.0f, 2.0f);

	// 初期ターゲット位置設定
	targetPosition_ = { 0.0f, 0.0f, 0.0f };

	enemies_.clear();
}

void EnemyManager::Update(float deltaTime) {
	// 全ての敵を更新
	for (auto& enemy : enemies_) {
		if (enemy) {
			enemy->Update(deltaTime);
		}
	}

	// 死んでいる敵を削除
	enemies_.erase(
		std::remove_if(enemies_.begin(), enemies_.end(),
			[](const std::unique_ptr<Enemy>& enemy) {
				return enemy && !enemy->IsAlive();
			}),
		enemies_.end()
	);
}

void EnemyManager::Draw() {
	
}

void EnemyManager::DrawImGui() {
#ifdef USE_IMGUI
	//ImGui::Begin("EnemyManager");
	//
	//ImGui::Text("Alive Enemies: %d", GetAliveEnemyCount());
	//ImGui::Text("Total Enemies: %zu", enemies_.size());
	//
	//ImGui::Separator();
	//
	//ImGui::DragFloat3("Target Position", &targetPosition_.x, 0.1f);
	//ImGui::DragFloat("Spawn Range Min", &spawnRangeMin_, 0.1f, 1.0f, 100.0f);
	//ImGui::DragFloat("Spawn Range Max", &spawnRangeMax_, 0.1f, 1.0f, 100.0f);
	//
	//ImGui::Separator();
	//
	//// 敵生成テスト
	//static int spawnCount = 5;
	//static int enemyTypeIndex = 0;
	//static int spawnTypeIndex = 0;
	//static int testWave = 1;
	//
	//ImGui::InputInt("Spawn Count", &spawnCount);
	//ImGui::Combo("Enemy Type", &enemyTypeIndex, "Normal\0Speed\0");
	//ImGui::Combo("Spawn Type", &spawnTypeIndex, "Scatter\0Concentration\0");
	//ImGui::InputInt("Test Wave", &testWave);
	//
	//if (ImGui::Button("Generate Enemies")) {
	//	Generate(
	//		static_cast<EnemyType>(enemyTypeIndex),
	//		static_cast<SpawnType>(spawnTypeIndex),
	//		spawnCount,
	//		static_cast<uint32_t>(testWave)
	//	);
	//}
	//
	//ImGui::SameLine();
	//
	//if (ImGui::Button("Clear All")) {
	//	Clear();
	//}
	//
	//ImGui::End();
#endif
}

void EnemyManager::Generate(EnemyType type, SpawnType spawnType, int count, uint32_t currentWave) {
	if (count <= 0) {
		return;
	}

	switch (spawnType) {
	case SpawnType::Scatter:
		// バラバラな個所に生成
		for (int i = 0; i < count; ++i) {
			Vector3 spawnPos = CalculateScatterPosition(currentWave);

			auto enemy = std::make_unique<Enemy>();
			enemy->Initialize();
			enemy->Setup(type, spawnPos, targetPosition_);

			enemies_.push_back(std::move(enemy));
		}
		break;

	case SpawnType::Concentration:
		// ある程度纏まって生成
		// まず基準位置を決定
		Vector3 basePos = CalculateScatterPosition(currentWave);

		for (int i = 0; i < count; ++i) {
			Vector3 spawnPos = CalculateConcentrationPosition(basePos, i, count);

			auto enemy = std::make_unique<Enemy>();
			enemy->Initialize();
			enemy->Setup(type, spawnPos, targetPosition_);

			enemies_.push_back(std::move(enemy));
		}
		break;
	}
}

int EnemyManager::GetAliveEnemyCount() const {
	int count = 0;
	for (const auto& enemy : enemies_) {
		if (enemy && enemy->IsAlive()) {
			++count;
		}
	}
	return count;
}

void EnemyManager::Clear() {
	enemies_.clear();
}

Vector3 EnemyManager::CalculateScatterPosition(uint32_t currentWave) {
	// ターゲット位置を中心にランダムな角度と距離で生成位置を決定
	float angle = angleDist_(gen_);
	float range = rangeDist_(gen_);

	// Waveが進むにつれて生成範囲を少し変化させる
	range += static_cast<float>(currentWave) * 0.5f;

	Vector3 spawnPos;
	spawnPos.x = targetPosition_.x + std::cos(angle) * range;
	spawnPos.y = targetPosition_.y;
	spawnPos.z = targetPosition_.z + std::sin(angle) * range;

	return spawnPos;
}

Vector3 EnemyManager::CalculateConcentrationPosition(const Vector3& basePosition, int index, int count) {
	// 基準位置の周りに円形または格子状に配置
	// 円形配置の例
	float angleOffset = static_cast<float>(2.0 * std::numbers::pi) / static_cast<float>(count);
	float angle = angleOffset * static_cast<float>(index);
	float radius = 3.0f; // 集まる範囲の半径

	// 少しランダム性を加える
	float randomX = concentrationDist_(gen_);
	float randomZ = concentrationDist_(gen_);

	Vector3 spawnPos;
	spawnPos.x = basePosition.x + std::cos(angle) * radius + randomX;
	spawnPos.y = basePosition.y;
	spawnPos.z = basePosition.z + std::sin(angle) * radius + randomZ;

	return spawnPos;
}
