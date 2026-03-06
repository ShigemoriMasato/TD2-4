#include "Bullet.h"

void Bullet::Initialize(float size) {
	IAttackObject::Initialize(size);
	collCircle_ = std::make_unique<Circle>();
	CollConfig config{};
	config.colliderInfo = collCircle_.get();
	SetCollider(config);
	collCircle_->radius = size_ / 2.0f;
}

void Bullet::SetConfig(Vector2 startPos, Vector2 direction, float speed) {
	collCircle_->center = startPos;
	direction_ = direction.Normalize();
	speed_ = speed;
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
	info.scale = { size_, size_, size_ };
	info.modelIndex = 2; // 仮のモデルインデックス
	info.color = 0xffff00ff; // 黄色
	return info;
}
