#pragma once
#include "IAttackObject.h"

class Bullet : public IAttackObject {
public:

	void Initialize(float size) override;
	void SetConfig(Vector2 startPos, Vector2 direction, float speed);
	void Update(float deltaTime) override;

	DrawInfo GetDrawInfo() override;

private:

	std::unique_ptr<Circle> collCircle_;
	Vector2 direction_;
	float speed_ = 0.0f;
	const float lifeTime_ = 0.5f; // 弾の寿命
	float timer_ = 0.0f;

};
