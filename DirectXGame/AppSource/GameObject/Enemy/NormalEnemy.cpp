#include "NormalEnemy.h"

void NormalEnemy::Initialize(Vector3* playerPos, EnemyManager* manager, int id) {
	IEnemy::Initialize(playerPos, manager, id);
	speed_ = float(rand() % 100) / 50.0f + baseSpeed_;
}

void NormalEnemy::Update(float deltaTime) {
	Vector3 direction = (*playerPos_ - drawInfo_.position).Normalize();
	drawInfo_.position += direction * speed_ * deltaTime;
	collCircle_->center = { drawInfo_.position.x, drawInfo_.position.z };
}
