#include "Swing.h"
#include <numbers>

void Swing::Initialize(const Config& config) {
	collDirCircle_ = std::make_unique<DirCircle>();
	collDirCircle_->center = { config.position.x + config.range, config.position.z + config.range };
	collDirCircle_->radius = radius_;
	collDirCircle_->direction = { cosf(config.direction), sinf(config.direction) };
	collDirCircle_->radian = std::numbers::pi_v<float> / 3.0f;
	direction_ = config.direction;

	config_ = config;

	CollConfig collConfig;
	collConfig.ownTag = CollTag::Attack;
	collConfig.targetTag = static_cast<uint32_t>(CollTag::Enemy);
	collConfig.colliderInfo = collDirCircle_.get();
	Collider::Initialize();
	SetCollider(collConfig);
}

void Swing::Update(float deltaTime) {
	// 攻撃の持続時間が終わったら非アクティブにする
	timer_ += deltaTime;
	if (timer_ >= lifeTime_) {
		isActive_ = false;
	}
}

DrawInfo Swing::GetDrawInfo() {
	DrawInfo info;
	return info;
}
