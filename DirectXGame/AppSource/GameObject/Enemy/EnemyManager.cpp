#include "EnemyManager.h"
#include "NormalEnemy.h"
#include "FastEnemy.h"
#include "TackleEnemy.h"
#include "IEnemy.h"
#include <GameObject/Map/Map.h>

void EnemyManager::Initialize(Vector3* playerPos, Map* map) {
	playerPos_ = playerPos;
	map_ = map;
	enemies_.clear();
	pendingEnemies_.clear();
}

void EnemyManager::Update(float deltaTime, Matrix4x4 vpMatrix, Matrix4x4 orthoVpMatrix) {
	std::vector<int> toSpawn;
	for (size_t i = 0; i < pendingEnemies_.size(); ++i) {
		pendingEnemies_[i].timer -= deltaTime;
		if (pendingEnemies_[i].timer <= 0.0f) {
			toSpawn.push_back(static_cast<int>(i));
		}
	}
	
	for (auto it = toSpawn.rbegin(); it != toSpawn.rend(); ++it) {
		int index = *it;
		auto& p = pendingEnemies_[index];
		
		int id = nextEnemyId_++;
		auto& enemy = enemies_[id];
		if (p.type == EnemyType::Fast) {
			enemy = std::make_unique<FastEnemy>();
		} else if (p.type == EnemyType::Tackle) {
			enemy = std::make_unique<TackleEnemy>();
		} else {
			enemy = std::make_unique<NormalEnemy>();
		}
		enemy->Initialize(playerPos_, this, id, map_);
		enemy->SetPosition(p.pos);
		enemy->SetHP(p.hp);
		enemy->SetAttack(p.attack);
		
		pendingEnemies_.erase(pendingEnemies_.begin() + index);
	}

	if (enemies_.size() == 0) {
		return;
	}

	std::vector<int> toRemove;
	for (auto& [id, enemy] : enemies_) {
		enemy->SetVPMatrix(vpMatrix);
		enemy->SetOrthoVPMatrix(orthoVpMatrix);
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

void EnemyManager::PopEnemy(Vector3 initPos, int hp, EnemyType type, float attack) {
	PendingEnemy pending;
	pending.pos = initPos;
	pending.hp = hp;
	pending.timer = 1.0f; // 1 second warning
	pending.type = type;
	pending.attack = attack;
	pendingEnemies_.push_back(pending);
}

std::vector<DrawInfo> EnemyManager::GetEnemyDrawInfos() const {
	std::vector<DrawInfo> drawInfos;
	int totalSize = int(pendingEnemies_.size() + enemies_.size() * 4);
	drawInfos.reserve(totalSize); // 敵の数に応じて適切な容量を確保
	for (const auto& p : pendingEnemies_) {
		DrawInfo info{};
		info.position = p.pos;
		info.rotation = {0, 0, 0};
		float scale = p.timer; // scale 1.0 to 0.0
		info.scale = {scale, scale, scale};
		
		if (p.type == EnemyType::Normal) {
			info.color = 0xff0000ff; // Red for Normal
		} else if (p.type == EnemyType::Fast) {
			info.color = 0x0000ffff; // Blue for Fast
		} else if (p.type == EnemyType::Tackle) {
			info.color = 0xffa500ff; // Orange for Tackle
		} else {
			info.color = 0xffffffff; // Default
		}

		auto modelManager = IEnemy::GetModelManager();
		if (modelManager) {
			info.modelIndex = modelManager->LoadModel("Cross"); 
		}
		
		drawInfos.push_back(info);
	}

	for (const auto& [id, enemy] : enemies_) {
		auto drawInfo = enemy->GetDrawInfos();
		drawInfos.insert(drawInfos.end(), drawInfo.begin(), drawInfo.end());
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
	pendingEnemies_.clear();
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
