#include "FastEnemy.h"

void FastEnemy::Initialize(Vector3* playerPos, EnemyManager* manager, int id, Map* map) {
	IEnemy::Initialize(playerPos, manager, id, map);
	speed_ = float(rand() % 100) / 50.0f + baseSpeed_;
	for (auto& drawInfo : drawInfo_) {
		drawInfo.scale = {0.75f, 0.75f, 0.75f};
		drawInfo.color = 0x0000ffff; // Blue color for distinction
	}
	SetModel("Fast");
}

void FastEnemy::Update(float deltaTime) {
	IEnemy::Update(deltaTime);
	Vector3 direction = (*playerPos_ - drawInfo_[0].position).Normalize();
	position_ += direction * speed_ * deltaTime;
	ClampPositionToMap();
}
