#pragma once
#include "IEnemy.h"
#include "GameObject/EasingAnimation/AnimationBundle.h"

enum class AnimState{
	Forward,
	Backward
};

class NormalEnemy : public IEnemy {
public:

	void Initialize(Vector3* playerPos, EnemyManager* manager, int id, Map* map) override;
	void Update(float deltaTime) override;

private:

	// 待機時のアニメーション
	void IdleAnimation(float deltaTime);

private:

	float speed_ = 1.0f;
	static inline float baseSpeed_ = 2.0f;
	
	// アニメーション用変数
	AnimationBundle<Vector3> posAnimForward_;
	AnimationBundle<Vector3> posAnimBackward_;
	Vector3 scale_ = {0.75f, 0.75f, 0.75f};
	AnimState state_ = AnimState::Forward;

};
