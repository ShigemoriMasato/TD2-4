#include "EnemyManager.h"
#include "NormalEnemy.h"
#include "FastEnemy.h"
#include "TackleEnemy.h"
#include "IEnemy.h"
#include <GameObject/Map/Map.h>
#include <Utility/ConvertString.h>
#include <GameObject/Enemy/EnemyEffect/EnemyEffectManager.h>

int EnemyManager::killCount = 0;

void EnemyManager::Initialize(Vector3* playerPos, Map* map, SHEngine::DrawData& plane, SHEngine::ModelManager* modelManager, EnemyEffect* enemyEffect) {
	playerPos_ = playerPos;
	map_ = map;
	enemyEffectManager_ = enemyEffect;
	enemies_.clear();
	pendingEnemies_.clear();

	damageText_.reserve(maxDamageTextNum_);
	isUsedText_.resize(maxDamageTextNum_, 0);
	damageTextPositions_.resize(maxDamageTextNum_);
	damageTextVelocities_.resize(maxDamageTextNum_);
	damageTextTimers_.resize(maxDamageTextNum_, 0.0f);
	for (int i = 0; i < maxDamageTextNum_; ++i) {
		auto text = std::make_unique<SHEngine::Text>();
		text->Initialize(plane, "YDWbananaslipplus.otf");
		text->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
		text->SetSize(0.05f);
		damageText_.push_back(std::move(text));
	}

	modelManager_ = modelManager;
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

	std::vector<int> toRemove;
	std::vector<std::pair<Vector3, std::vector<int>>> damageQueue;
	for (auto& [id, enemy] : enemies_) {
		enemy->SetVPMatrix(vpMatrix);
		enemy->SetOrthoVPMatrix(orthoVpMatrix);
		enemy->Update(deltaTime);
		enemy->UpdateCollider();
		auto damages = enemy->GetDamageQueue();

		if (!damages.empty()) {
			auto pos = enemy->GetPosition();
			damageQueue.push_back({ pos, damages });
		}

		if (!enemy->IsActive()) {
			Vector3 enemyPos = enemy->GetPosition();
			enemyPos.y -= 1.0f;
			Vector3 scale = { 1.0f, 1.0f, 1.0f };
			Vector3 rotate = { 0.0f, 0.0f, 0.0f };
			Matrix4x4 enemyWorldMatrix = Matrix::MakeAffineMatrix(scale, rotate, enemyPos);
      
			enemyEffectManager_->CreateDeathEffect1(enemyWorldMatrix);
			
      toRemove.push_back(id);
		}
	}

	for (int id : toRemove) {
		Kill(id);
	}

	CreateDamageText(damageQueue);

	for (int i = 0; i < int(damageText_.size()); ++i) {
		if (isUsedText_[i] == 0) {
			continue;
		}

		auto& text = damageText_[i];
		Vector3& pos = damageTextPositions_[i];
		Vector3& vel = damageTextVelocities_[i];
		float& timer = damageTextTimers_[i];
		timer += deltaTime;
		vel.y -= 19.6f * deltaTime; // accelerate upwards
		pos += vel * deltaTime;
		if (timer >= damageLifeTime_) {
			isUsedText_[i] = 0;
			continue;
		}
		text->SetTransform({ {1.0f, 1.0f, 1.0f}, {0.0f,0.0f ,0.0f}, pos });
		text->Update(vpMatrix);
	}
}

void EnemyManager::Draw(CmdObj* cmdObj) {
	for (size_t i = 0; i < damageText_.size(); ++i) {
		if (isUsedText_[i]) {
			damageText_[i]->Draw(cmdObj);
		}
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
	pending.timer = 1.5f; // 1 second warning
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
		info.rotation = { 0, 0, 0 };
		float scale = p.timer; // scale 1.0 to 0.0
		info.scale = { scale, scale, scale };

		if (p.type == EnemyType::Normal) {
			info.color = 0xff0000ff; // Red for Normal
		} else if (p.type == EnemyType::Fast) {
			info.color = 0x0000ffff; // Blue for Fast
		} else if (p.type == EnemyType::Tackle) {
			info.color = 0xffa500ff; // Orange for Tackle
		} else {
			info.color = 0xffffffff; // Default
		}

		if (modelManager_) {
			info.modelIndex = modelManager_->LoadModel("Cross");
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
	killCount++;
}

void EnemyManager::CreateDamageText(std::vector<std::pair<Vector3, std::vector<int>>> damageQueue) {
	size_t i;
	for (const auto& [pos, damages] : damageQueue) {
		for (int damage : damages) {
			for (i = 0; i < isUsedText_.size(); ++i) {
				if (isUsedText_[i] == 0) {
					isUsedText_[i] = 1;
					damageText_[i]->SetText(ConvertString(std::to_string(damage)));
					damageTextPositions_[i] = pos;
					damageTextPositions_[i].y += 3.0f;
					Vector3 tmpVel = { 0.0f, 5.0f, 0.0f };
					tmpVel.x = rand() % 400 / 50.0f - 2.0f; // -2.0 to 2.0
					tmpVel.z = rand() % 400 / 50.0f - 2.0f; // -2.0 to 2.0
					damageTextVelocities_[i] = tmpVel.Normalize() * 8.0f;
					damageTextTimers_[i] = 0.0f;
					break;
				}
			}
		}
	}
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
