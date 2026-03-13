#include "NormalEnemy.h"

void NormalEnemy::Initialize(Vector3* playerPos, EnemyManager* manager, int id) {
	IEnemy::Initialize(playerPos, manager, id);
	speed_ = float(rand() % 100) / 50.0f + baseSpeed_;
	drawInfo_.scale = { 0.75f, 0.75f, 0.75f };
	SetModel("Normal");
}

void NormalEnemy::Update(float deltaTime) {
	Vector3 direction = (*playerPos_ - drawInfo_.position).Normalize();
	position_ += direction * speed_ * deltaTime;
}
