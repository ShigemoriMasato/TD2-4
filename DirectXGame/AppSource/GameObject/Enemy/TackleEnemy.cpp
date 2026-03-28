#include "TackleEnemy.h"
#include <cstdlib>
#include <ctime>
void TackleEnemy::Initialize(Vector3* playerPos, EnemyManager* manager, int id) {
    IEnemy::Initialize(playerPos, manager, id);
    speed_ = float(rand() % 100) / 50.0f + baseApproachSpeed_;
    drawInfo_.scale = { 1.f, 1.f, 1.f };
    SetModel("Tackle");
    drawInfo_.color = 0xffff00ff; // Yellow color
    state_ = State::Approach;
}

void TackleEnemy::Update(float deltaTime) {
    IEnemy::Update(deltaTime);

    Vector3 toPlayer = *playerPos_ - drawInfo_.position;
    float distance = toPlayer.Length();
    Vector3 direction = toPlayer.Normalize();

    switch (state_) {
    case State::Approach:
        position_ += direction * speed_ * deltaTime;
        if (distance < 4.0f) {
            state_ = State::StepBack;
            stateTimer_ = 0.5f; // Step back for 0.5 seconds
        }
        break;
    case State::StepBack:
        position_ -= direction * baseStepBackSpeed_ * deltaTime;
        stateTimer_ -= deltaTime;
        if (stateTimer_ <= 0.0f) {
            state_ = State::Charge;
            stateTimer_ = 0.8f; // Charge for 0.8 seconds
            chargeDirection_ = direction; // Lock the charge direction
        }
        break;
    case State::Charge:
        position_ += chargeDirection_ * baseChargeSpeed_ * deltaTime;
        stateTimer_ -= deltaTime;
        if (stateTimer_ <= 0.0f) {
            state_ = State::Approach;
            speed_ = float(rand() % 100) / 50.0f + baseApproachSpeed_; // Reset approach speed just in case
        }
        break;
    }
}
