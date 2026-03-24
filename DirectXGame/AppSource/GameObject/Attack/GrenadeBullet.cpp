#include "GrenadeBullet.h"
#include <random>

void GrenadeBullet::Initialize(const Config& config) {
	config_ = config;

	collCircle_ = std::make_unique<Circle>();
	collCircle_->center = {config.position.x, config.position.z};
	collCircle_->radius = baseRadius_;

	CollConfig collConfig;
	collConfig.ownTag = CollTag::Attack;
	collConfig.targetTag = static_cast<uint32_t>(CollTag::Enemy);
	collConfig.colliderInfo = collCircle_.get();
	Collider::Initialize();
	SetCollider(collConfig);

	static std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> spreadDist(-config.spreadAngle / 2.0f, config.spreadAngle / 2.0f);
	float radian = config.direction + spreadDist(rng);

	direction_ = {cosf(radian), sinf(radian)};
	speed_ = config.speed;
}

void GrenadeBullet::Update(float deltaTime) {
	if (state_ == State::Rolling) {
		speed_ -= speed_ * 2.0f * deltaTime;
		if (speed_ < 0.0f)
			speed_ = 0.0f;

		collCircle_->center += direction_ * speed_ * deltaTime;

		timer_ += deltaTime;
		if (timer_ >= rollingTime_) {
			Explode();
		}
	} else if (state_ == State::Exploding) {
		// 爆発中は移動せず、時間が来たら消滅
		timer_ += deltaTime;
		if (timer_ >= explosionTime_) {
			isActive_ = false;
		}
	}
}

void GrenadeBullet::Explode() {
	if (state_ == State::Exploding)
		return;

	state_ = State::Exploding;
	timer_ = 0.0f;
	speed_ = 0.0f;

	collCircle_->radius = explosionRadius_;
}

DrawInfo GrenadeBullet::GetDrawInfo() {
	DrawInfo info;
	info.position = {collCircle_->center.x, 0.0f, collCircle_->center.y};

	if (state_ == State::Rolling) {
		info.scale = {baseRadius_ * 2.0f, baseRadius_ * 2.0f, baseRadius_ * 2.0f};
		info.color = 0xff4040ff;
		info.modelIndex = 2;
	} else {
		info.scale = {explosionRadius_ * 2.0f, explosionRadius_ * 2.0f, explosionRadius_ * 2.0f};
		info.color = 0xffff40ff;
		info.modelIndex = 2;
	}
	return info;
}

void GrenadeBullet::OnCollision(Collider* other) {
	int id = other->GetID();

	for (int hitId : hitEnemyIds_) {
		if (hitId == id)
			return;
	}

	if (state_ == State::Rolling) {
		// 転がっている最中に敵にぶつかったら即爆発
		Explode();
		hitEnemyIds_.push_back(id);
	} else if (state_ == State::Exploding) {
		// 爆発中は、貫通回数を無視して範囲内にいる敵すべてにダメージを与える
		hitEnemyIds_.push_back(id);
	}
}