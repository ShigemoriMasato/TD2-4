#include "EnemyManager.h"
#include "NormalEnemy.h"
#include <algorithm>
#include <cmath>

void EnemyManager::Initialize(Vector3* playerPos) {
	playerPos_ = playerPos;
	enemies_.clear();
	spawnWarnings_.clear();
}

void EnemyManager::Update(float deltaTime) {
	for (auto it = spawnWarnings_.begin(); it != spawnWarnings_.end();) {
		it->timer -= deltaTime;
		
		// スポーンに近づくにつれて（timerが0に近づくにつれて）Scaleを0.0に近づける
		float scale = std::max(it->timer, 0.0f);
		it->drawInfo.scale = {scale, scale, scale};

		if (it->timer <= 0.0f) {
			PopEnemy(it->pos, it->hp, it->type);
			it = spawnWarnings_.erase(it);
		} else {
			++it;
		}
	}

	if (enemies_.size() == 0) {
		return;
	}

	std::vector<int> toRemove;
	for (auto& [id, enemy] : enemies_) {
		enemy->Update(deltaTime);
		enemy->UpdateCollider();
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

void EnemyManager::PopEnemy(Vector3 initPos, int hp, EnemyType type) {
	int id = nextEnemyId_++;
	auto& enemy = enemies_[id];
	float enemyHp = static_cast<float>(hp);

	switch (type) {
	case EnemyType::Fast:
		enemy = std::make_unique<FastEnemy>();
		enemyHp *= 0.5f;
		break;
	case EnemyType::Normal:
	default:
		enemy = std::make_unique<NormalEnemy>();
		break;
	}

	enemy->Initialize(playerPos_, this, id);
	enemy->SetPosition(initPos);
	enemy->SetHP(std::max(enemyHp, 0.5f));
}

void EnemyManager::AddWarning(Vector3 pos, int hp, EnemyType type) {
	SpawnWarning warning;
	warning.pos = pos;
	warning.hp = hp;
	warning.type = type;
	warning.timer = 1.0f; // 1秒後にスポーン
	
	warning.drawInfo.position = pos;
	warning.drawInfo.scale = {1.0f, 1.0f, 1.0f};
	warning.drawInfo.rotation = {0.0f, 0.0f, 0.0f};
	warning.drawInfo.modelIndex = IEnemy::GetModelManager()->LoadModel("Assets/Model/Cross"); 
	
	spawnWarnings_.push_back(warning);
}

std::vector<DrawInfo> EnemyManager::GetEnemyDrawInfos() const {
	std::vector<DrawInfo> drawInfos;
	for (const auto& w : spawnWarnings_) {
		drawInfos.push_back(w.drawInfo);
	}
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

void EnemyManager::Clear() {
	enemies_.clear();
	nextEnemyId_ = 0;
}

void EnemyManager::RemoveEnemy(IEnemy* target) {
	if (!target) {
		return;
	}

	// 敵ポインタと比較して削除
	for (const auto& [id, enemy] : enemies_) {
		if (enemy.get() == target) {
			enemies_.erase(id);
			return;
		}
	}
}
