#pragma once
#include "IEnemy.h"

class Map;

enum class EnemyType {
	Normal,
	Fast,
	Tackle
};

struct PendingEnemy {
	Vector3 pos;
	int hp;
	float timer;
	EnemyType type;
	float attack;
};

class EnemyManager {
public:
	void Initialize(Vector3* playerPos, Map* map = nullptr);
	void Update(float deltaTime, Matrix4x4 vpMatrix, Matrix4x4 orthoVpMatrix);
	void DrawImGui();

	void PopEnemy(Vector3 initPos = {0.0f, 0.0f, 0.0f}, int hp = 1, EnemyType type = EnemyType::Normal, float attack = 1.0f);

	std::vector<DrawInfo> GetEnemyDrawInfos() const;
	std::vector<IEnemy*> GetEnemies() const;
	std::vector<DrawInfo> GetPendingDrawInfos() const;
	void RemoveEnemy(IEnemy* target);

	void Clear();

	void Kill(int id);

private:

	Vector3* playerPos_ = nullptr;
	Map* map_ = nullptr;
	std::map<int, std::unique_ptr<IEnemy>> enemies_;
	std::vector<PendingEnemy> pendingEnemies_;

	int nextEnemyId_ = 0;

};
