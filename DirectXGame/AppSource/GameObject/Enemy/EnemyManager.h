#pragma once
#include "IEnemy.h"

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
	float attack; // 追加
};

class EnemyManager {
public:
	void Initialize(Vector3* playerPos);
	void Update(float deltaTime, Matrix4x4 vpMatrix, Matrix4x4 orthoVpMatrix);
	void DrawImGui();

	void PopEnemy(Vector3 initPos = {0.0f, 0.0f, 0.0f}, int hp = 1, EnemyType type = EnemyType::Normal, float attack = 1.0f); // 追加

	std::vector<DrawInfo> GetEnemyDrawInfos() const;
	std::vector<IEnemy*> GetEnemies() const;
	std::vector<DrawInfo> GetPendingDrawInfos() const;
	/// @brief 謨ｵ繧貞炎髯､
	/// @param target 蜑企勁縺吶ｋ謨ｵ
	void RemoveEnemy(IEnemy* target);

	/// @brief 蜈ｨ縺ｦ縺ｮ謨ｵ繧偵け繝ｪ繧｢
	void Clear();

	void Kill(int id);

private:

	Vector3* playerPos_ = nullptr;
	std::map<int, std::unique_ptr<IEnemy>> enemies_;
	std::vector<PendingEnemy> pendingEnemies_;

	int nextEnemyId_ = 0;

};
