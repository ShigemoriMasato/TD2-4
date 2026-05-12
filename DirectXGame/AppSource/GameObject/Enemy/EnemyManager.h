#pragma once
#include "IEnemy.h"

class Map;
class EnemyEffect;

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
	void Initialize(Vector3* playerPos, Map* map, SHEngine::DrawData& plane, SHEngine::ModelManager* modelManager);
	void SetEnemyEffect(EnemyEffect* enemyEffect) { enemyEffect_ = enemyEffect; }
	void Update(float deltaTime, Matrix4x4 vpMatrix, Matrix4x4 orthoVpMatrix);
	void Draw(CmdObj* cmdObj);
	void DrawImGui();

	void PopEnemy(Vector3 initPos = {0.0f, 0.0f, 0.0f}, int hp = 1, EnemyType type = EnemyType::Normal, float attack = 1.0f);

	std::vector<DrawInfo> GetEnemyDrawInfos() const;
	std::vector<IEnemy*> GetEnemies() const;
	void RemoveEnemy(IEnemy* target);

	void Clear();

	void Kill(int id);

	const int GetKillCount() const { return killCount; }

private:

	void CreateDamageText(std::vector<std::pair<Vector3, std::vector<int>>>);

	SHEngine::ModelManager* modelManager_ = nullptr;

	Vector3* playerPos_ = nullptr;
	Map* map_ = nullptr;
	EnemyEffect* enemyEffect_;
	std::map<int, std::unique_ptr<IEnemy>> enemies_;
	std::vector<PendingEnemy> pendingEnemies_;

	int nextEnemyId_ = 0;

	std::vector<std::unique_ptr<SHEngine::Text>> damageText_;
	std::vector<int> isUsedText_;
	std::vector<Vector3> damageTextPositions_;
	std::vector<Vector3> damageTextVelocities_;
	std::vector<float> damageTextTimers_;
	const float damageLifeTime_ = 0.7f;
	const int maxDamageTextNum_ = 64;
	static int killCount;
};
