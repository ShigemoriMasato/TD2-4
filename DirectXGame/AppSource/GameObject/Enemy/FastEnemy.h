#pragma once
#include "IEnemy.h"

class FastEnemy : public IEnemy {
public:

    void Initialize(Vector3* playerPos, EnemyManager* manager, int id, Map* map) override;
    void Update(float deltaTime) override;

private:

    float speed_ = 1.0f;
    static inline float baseSpeed_ = 5.0f;

};
