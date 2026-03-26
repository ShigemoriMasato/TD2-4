#include "FastEnemy.h"

void FastEnemy::Initialize(Vector3* playerPos, EnemyManager* manager, int id) {
    IEnemy::Initialize(playerPos, manager, id);
    speed_ = float(rand() % 100) / 50.0f + baseSpeed_;
    drawInfo_.scale = { 0.75f, 0.75f, 0.75f };
    SetModel("Fast");
    drawInfo_.color = 0x0000ffff; // Blue color for distinction
}

void FastEnemy::Update(float deltaTime) {
    IEnemy::Update(deltaTime);
    Vector3 direction = (*playerPos_ - drawInfo_.position).Normalize();
    position_ += direction * speed_ * deltaTime;
}
