#pragma once
#include "IEnemy.h"

class EnemyManager {
public:
	void Initialize(Vector3* playerPos);
	void Update(float deltaTime);
	void DrawImGui();

	void PopEnemy(Vector3 initPos = {0.0f, 0.0f, 0.0f});

	std::vector<DrawInfo> GetEnemyDrawInfos() const;

	void Kill(int id);

private:

	Vector3* playerPos_ = nullptr;
	std::map<int, std::unique_ptr<IEnemy>> enemies_;

	int nextEnemyId_ = 0;

};
