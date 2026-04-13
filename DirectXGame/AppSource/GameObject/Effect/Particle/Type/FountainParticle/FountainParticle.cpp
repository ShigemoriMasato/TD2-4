#include "FountainParticle.h"
#include <random>
#include <Utility/MatrixFactory.h>

namespace
{
	Vector3 RandInAABB(const Vector3& min, const Vector3& max)
	{
		return Vector3(
			RandomUtils::RangeFloat(min.x, max.x),
			RandomUtils::RangeFloat(min.y, max.y),
			RandomUtils::RangeFloat(min.z, max.z)
		);
	}
}


void FountainParticle::Initialize(SHEngine::TextureManager* textureManager, SHEngine::ModelManager* modelManager)
{
	particle_.Initialize(textureManager, modelManager);
}

void FountainParticle::SetConfig(const FountainConfig& config)
{
	config_ = config;
	particle_.SetModel(config.cfg.modelPath);
	particle_.SetTexture(config.cfg.texturePath);
}

void FountainParticle::Update(float dt)
{
	// 「いま生きてる粒が無い」かつ「emitも止まってる」なら何もしない
	if (!isActive_ && instances_.empty())
	{
		particle_.Clear();
		return;
	}

	particle_.Clear();

	emitTimer_ += dt;

	// 一定時間ごとにパーティクルを発生させる
	if (isActive_ && emitTimer_ >= config_.cfg.emitInterval)
	{
		emitTimer_ = 0.0f;


		for (int32_t i = 0; i < config_.cfg.emitNum; ++i)
		{
			Matrix4x4 emitWorld = Matrix4x4::Identity();

			ParticleSRT s;
			ParticleSRT r;
			ParticleSRT t;

			if (config_.translate.isRandom_value) t.initial.value = RandInAABB(config_.translate.randomRange_value_min, config_.translate.randomRange_value_max);
			else t.initial.value = config_.translate.initial.value;
			if (config_.translate.isRandom_velocity) t.initial.velocity = RandInAABB(config_.translate.randomRange_velocity_min, config_.translate.randomRange_velocity_max);
			else t.initial.velocity = config_.translate.initial.velocity;
			if (config_.translate.isRandom_acceleration) t.initial.acceleration = RandInAABB(config_.translate.randomRange_acceleration_min, config_.translate.randomRange_acceleration_max);
			else t.initial.acceleration = config_.translate.initial.acceleration;

			if (config_.rotate.isRandom_value) r.initial.value = RandInAABB(config_.rotate.randomRange_value_min, config_.rotate.randomRange_value_max);
			else r.initial.value = config_.rotate.initial.value;
			if (config_.rotate.isRandom_velocity) r.initial.velocity = RandInAABB(config_.rotate.randomRange_velocity_min, config_.rotate.randomRange_velocity_max);
			else r.initial.velocity = config_.rotate.initial.velocity;
			if (config_.rotate.isRandom_acceleration) r.initial.acceleration = RandInAABB(config_.rotate.randomRange_acceleration_min, config_.rotate.randomRange_acceleration_max);
			else r.initial.acceleration = config_.rotate.initial.acceleration;

			if (config_.scale.isRandom_value) s.initial.value = RandInAABB(config_.scale.randomRange_value_min, config_.scale.randomRange_value_max);
			else s.initial.value = config_.scale.initial.value;
			if (config_.scale.isRandom_velocity) s.initial.velocity = RandInAABB(config_.scale.randomRange_velocity_min, config_.scale.randomRange_velocity_max);
			else s.initial.velocity = config_.scale.initial.velocity;
			if (config_.scale.isRandom_acceleration) s.initial.acceleration = RandInAABB(config_.scale.randomRange_acceleration_min, config_.scale.randomRange_acceleration_max);
			else s.initial.acceleration = config_.scale.initial.acceleration;

			instances_.push_back({ s,r,t,0.0f, Vector4(1,1,1,1) });
		}
	}

	// 年齢更新 & 老人削除
	for (auto& instance : instances_)
	{
		instance.age += dt;
	}
	instances_.erase(
		std::remove_if(instances_.begin(), instances_.end(),
			[this](const ParticleInstance& p) { return p.age >= config_.cfg.lifeTime; }),
		instances_.end());
	instances_.erase(
		std::remove_if(instances_.begin(), instances_.end(),
			[this](const ParticleInstance& p) { return p.scale.initial.value.x <= 0.0f || p.scale.initial.value.y <= 0.0f || p.scale.initial.value.z <= 0.0f; }),
		instances_.end());


	// SRT更新 & GPU転送データ作成
	for (auto& instance : instances_)
	{
		instance.translate.initial.value += instance.translate.initial.velocity * dt + 0.5f * instance.translate.initial.acceleration * dt * dt;
		instance.translate.initial.velocity += instance.translate.initial.acceleration * dt;
		instance.rotate.initial.value += instance.rotate.initial.velocity * dt + 0.5f * instance.rotate.initial.acceleration * dt * dt;
		instance.rotate.initial.velocity += instance.rotate.initial.acceleration * dt;
		instance.scale.initial.value += instance.scale.initial.velocity * dt + 0.5f * instance.scale.initial.acceleration * dt * dt;
		instance.scale.initial.velocity += instance.scale.initial.acceleration * dt;

		const Matrix4x4 world = Matrix::MakeAffineMatrix(instance.scale.initial.value, instance.rotate.initial.value, instance.translate.initial.value) * modelWorld_;

		particle_.pushInstance(world, instance.color);
	}
}


void FountainParticle::SetEnabled(bool isActive)
{
	isActive_ = isActive;
	if (!isActive_)
	{
		emitTimer_ = 0.0f;
	}
}
