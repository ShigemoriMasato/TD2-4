#pragma once
#include "IEnemy.h"

class NormalEnemy : public IEnemy {
public:

	void Initialize(Vector3* playerPos, EnemyManager* manager, int id) override;
	void Update(float deltaTime) override;

private:

	float speed_ = 2.0f;
	static inline float baseSpeed_ = 2.0f;

};
