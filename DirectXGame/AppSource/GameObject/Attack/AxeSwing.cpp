#include "AxeSwing.h"
#include <numbers>

void AxeSwing::Initialize(const Config& config) {
	collDirCircle_ = std::make_unique<DirCircle>();
	collDirCircle_->center = {config.position.x + config.range, config.position.z + config.range};
	collDirCircle_->radius = radius_;
	collDirCircle_->direction = {cosf(config.direction), sinf(config.direction)};

	collDirCircle_->radian = std::numbers::pi_v<float> / 1.5f;
	direction_ = config.direction;

	config_ = config;

	CollConfig collConfig;
	collConfig.ownTag = CollTag::Attack;
	collConfig.targetTag = static_cast<uint32_t>(CollTag::Enemy);
	collConfig.colliderInfo = collDirCircle_.get();
	Collider::Initialize();
	SetCollider(collConfig);
}

void AxeSwing::Update(float deltaTime) {
	timer_ += deltaTime;
	if (timer_ >= lifeTime_) {
		isActive_ = false;
	}
}

DrawInfo AxeSwing::GetDrawInfo() {
	DrawInfo info;
	info.position = {collDirCircle_->center.x, 0.0f, collDirCircle_->center.y};
	info.rotation = {0.0f, -direction_, 0.0f};
	info.scale = {radius_, 1.0f, radius_};
	info.modelIndex = modelManager_->LoadModel(modelPath_);
	info.color = 0xff804000;
	return info;
}