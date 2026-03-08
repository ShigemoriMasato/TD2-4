#include "Bullet.h"
#include <random>

void Bullet::Initialize(const Config& config) {
	config_ = config;

	//当たり判定
	collCircle_ = std::make_unique<Circle>();
	collCircle_->center = { config.position.x, config.position.z };
	collCircle_->radius = radius_ + 0.1f;	//ちょっと大きめにしておく
	CollConfig collConfig;
	collConfig.ownTag = CollTag::Attack;
	collConfig.targetTag = static_cast<uint32_t>(CollTag::Enemy);
	collConfig.colliderInfo = collCircle_.get();
	Collider::Initialize();
	SetCollider(collConfig);

	static std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> spreadDist(-config.spreadAngle / 2.0f, config.spreadAngle / 2.0f);

	float radian = config.direction + spreadDist(rng);

	direction_ = { cosf(radian), sinf(radian) };
	constexpr float lifeTime = 0.5f;
	speed_ = config.range * lifeTime;
}

void Bullet::Update(float deltaTime) {
	collCircle_->center += direction_ * speed_ * deltaTime;

	timer_ += deltaTime;
	if (timer_ >= lifeTime_) {
		isActive_ = false;
	}
}

DrawInfo Bullet::GetDrawInfo() {
	DrawInfo info;
	info.position = { collCircle_->center.x, 0.0f, collCircle_->center.y };
	info.scale = { radius_, radius_, radius_ };
	info.modelIndex = 2; // 仮のモデルインデックス
	info.color = 0xffff00ff; // 黄色
	return info;
}
