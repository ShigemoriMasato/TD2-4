#pragma once
#include "IAttackObject.h"

class Bullet : public IAttackObject {
public:

	void Initialize(const Config& config) override;
	void Update(float deltaTime) override;

	DrawInfo GetDrawInfo() override;

private:

	std::unique_ptr<Circle> collCircle_;
	Vector2 direction_;
	float speed_ = 0.0f;
	const float lifeTime_ = 0.5f; // 弾の寿命
	float timer_ = 0.0f;
	float radius_ = 0.3f; // 弾の半径
};
