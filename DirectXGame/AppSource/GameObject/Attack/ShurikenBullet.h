#pragma once
#include "IAttackObject.h"
#include <GameObject/Enemy/EnemyManager.h>

class ShurikenBullet : public IAttackObject {
public:
	void Initialize(const Config& config) override;
	void Update(float deltaTime) override;
	DrawInfo GetDrawInfo() override;
	void OnCollision(Collider* other) override;

	void SetEnemyManager(EnemyManager* manager) { enemyManager_ = manager; }

private:
	std::unique_ptr<Circle> collCircle_;
	Vector2 direction_;
	float speed_ = 0.0f;
	const float lifeTime_ = 0.8f;
	float timer_ = 0.0f;
	static constexpr float radius_ = 0.4f;

	int bounceCount_ = 0;
	EnemyManager* enemyManager_ = nullptr;

	float rotate_ = 0.0f;
};