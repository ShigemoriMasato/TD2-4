#pragma once
#include "IEnemy.h"

class TackleEnemy : public IEnemy {
public:

    void Initialize(Vector3* playerPos, EnemyManager* manager, int id, Map* map) override;
    void Update(float deltaTime) override;

    static void DrawImGui();

private:

    enum class State {
        Approach,
        StepBack,
        Charge
    };
    State state_ = State::Approach;

    float speed_ = 2.0f;
    static inline float baseApproachSpeed_ = 3.0f;
    static inline float baseStepBackSpeed_ = 5.0f;
    static inline float baseChargeSpeed_ = 15.0f;

    static inline float approachDistance_ = 5.0f;
    static inline float stepBackDuration_ = 0.75f;
    static inline float chargeDuration_ = 1.0f;

    float stateTimer_ = 0.0f;
    Vector3 chargeDirection_{};
};
