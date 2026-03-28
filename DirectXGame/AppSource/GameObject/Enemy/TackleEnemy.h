#pragma once
#include "IEnemy.h"

class TackleEnemy : public IEnemy {
public:

    void Initialize(Vector3* playerPos, EnemyManager* manager, int id) override;
    void Update(float deltaTime) override;

private:

    enum class State {
        Approach,
        StepBack,
        Charge
    };
    State state_ = State::Approach;

    float speed_ = 1.0f;
    static inline float baseApproachSpeed_ = 2.0f;
    static inline float baseStepBackSpeed_ = 1.5f;
    static inline float baseChargeSpeed_ = 8.0f;

    float stateTimer_ = 0.0f;
    Vector3 chargeDirection_{};
};
