#pragma once
#include "IAttackObject.h"

class GrenadeBullet : public IAttackObject {
public:
	void Initialize(const Config& config) override;
	void Update(float deltaTime) override;
	DrawInfo GetDrawInfo() override;
	void OnCollision(Collider* other) override;

private:
	// 爆発状態へ移行する関数
	void Explode();

	std::unique_ptr<Circle> collCircle_;
	Vector2 direction_;
	float speed_ = 0.0f;

	// 弾の状態を管理
	enum class State { Rolling, Exploding };
	State state_ = State::Rolling;

	float timer_ = 0.0f;
	const float rollingTime_ = 1.0f;
	const float explosionTime_ = 0.2f;

	static constexpr float baseRadius_ = 0.5f;
	static constexpr float explosionRadius_ = 4.0f;
};