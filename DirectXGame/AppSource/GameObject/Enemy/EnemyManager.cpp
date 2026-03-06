#include "EnemyManager.h"
#include "NormalEnemy.h"

void EnemyManager::Initialize(Vector3* playerPos) {
	playerPos_ = playerPos;
	enemies_.clear();
}

void EnemyManager::Update(float deltaTime) {
	if(enemies_.size() == 0) {
		return;
	}

	std::vector<int> toRemove;
	for (auto& [id, enemy] : enemies_) {
		enemy->Update(deltaTime);
		if (!enemy->IsActive()) {
			toRemove.push_back(id);
		}
	}

	for (int id : toRemove) {
		Kill(id);
	}
}

void EnemyManager::DrawImGui() {
#ifdef USE_IMGUI

	

#endif
}

void EnemyManager::PopEnemy(Vector3 initPos) {
	int id = nextEnemyId_++;
	auto& enemy = enemies_[id];
	enemy = std::make_unique<NormalEnemy>();
	enemy->Initialize(playerPos_, this, id);
	enemy->SetPosition(initPos);
}

std::vector<DrawInfo> EnemyManager::GetEnemyDrawInfos() const {
	std::vector<DrawInfo> drawInfos;
	for (const auto& [id, enemy] : enemies_) {
		drawInfos.push_back(enemy->GetDrawInfo());
	}
	return drawInfos;
}

std::vector<IEnemy*> EnemyManager::GetEnemies() const {
	std::vector<IEnemy*> enemies;
	for (const auto& [id, enemy] : enemies_) {
		enemies.push_back(enemy.get());
	}
	return enemies;
}

void EnemyManager::Kill(int id) {
	enemies_.erase(id);
}
