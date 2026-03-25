#include "EnemyManager.h"
#include "NormalEnemy.h"
#include "IEnemy.h"

void EnemyManager::Initialize(Vector3* playerPos) {
	playerPos_ = playerPos;
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
		enemy = std::make_unique<NormalEnemy>();
		enemy->Initialize(playerPos_, this, id);
		enemy->SetPosition(p.pos);
		enemy->SetHP(p.hp);
		
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

void EnemyManager::DrawUI(CmdObj* cmdObj){
	for (auto& [id, enemy] : enemies_) {
		enemy->DrawUI(cmdObj);
	}
}

void EnemyManager::PopEnemy(Vector3 initPos, int hp) {
	PendingEnemy pending;
	pending.pos = initPos;
	pending.hp = hp;
	pending.timer = 1.0f; // 1 second warning
	pendingEnemies_.push_back(pending);
}

std::vector<DrawInfo> EnemyManager::GetEnemyDrawInfos() const {
	std::vector<DrawInfo> drawInfos;
	for (const auto& p : pendingEnemies_) {
		DrawInfo info{};
		info.position = p.pos;
		info.rotation = {0, 0, 0};
		float scale = p.timer; // scale 1.0 to 0.0
		info.scale = {scale, scale, scale};
		info.color = 0xffffffff; // Ensure visible
		
		auto modelManager = IEnemy::GetModelManager();
		if (modelManager) {
			info.modelIndex = modelManager->LoadModel("Cross"); 
		}
		
		drawInfos.push_back(info);
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
