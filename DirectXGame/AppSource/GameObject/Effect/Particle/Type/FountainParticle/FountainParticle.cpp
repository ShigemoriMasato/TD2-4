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

void PhysicsParticle::Initialize(SHEngine::TextureManager* textureManager, SHEngine::ModelManager* modelManager)
{
	particle_.Initialize(textureManager, modelManager);
}


void PhysicsParticle::SetConfig(const PhysicsConfig& config)
{
	config_ = config;
	particle_.SetModel(config.cfg.modelPath);
	particle_.SetTexture(config.cfg.texturePath);
}

void PhysicsParticle::Update(float dt)
{
	// 「いま生きてる粒が無い」かつ「emitも止まってる」なら何もしない
	if (!isActive_ && instances_.empty())
	{
		particle_.Clear();
		return;
	}

	particle_.Clear();

	// 一定時間ごとにパーティクルを発生させる
	if (isActive_ && emitTimer_ >= config_.cfg.emitInterval)
	{
		emitTimer_ = 0.0f;


		for (int32_t i = 0; i < config_.cfg.emitNum; ++i)
		{
			ParticleSRT s;
			ParticleSRT r;
			ParticleSRT t;

			if (config_.translate.value.isRandom) t.value.baseValue = RandInAABB(config_.translate.value.randomRange_min, config_.translate.value.randomRange_max);
			else t.value.baseValue = config_.translate.value.baseValue;
			if (config_.translate.velocity.isRandom) t.velocity.baseValue = RandInAABB(config_.translate.velocity.randomRange_min, config_.translate.velocity.randomRange_max);
			else t.velocity.baseValue = config_.translate.velocity.baseValue;
			if (config_.translate.acceleration.isRandom) t.acceleration.baseValue = RandInAABB(config_.translate.acceleration.randomRange_min, config_.translate.acceleration.randomRange_max);
			else t.acceleration.baseValue = config_.translate.acceleration.baseValue;

			if (config_.rotate.value.isRandom) r.value.baseValue = RandInAABB(config_.rotate.value.randomRange_min, config_.rotate.value.randomRange_max);
			else r.value.baseValue = config_.rotate.value.baseValue;
			if (config_.rotate.velocity.isRandom) r.velocity.baseValue = RandInAABB(config_.rotate.velocity.randomRange_min, config_.rotate.velocity.randomRange_max);
			else r.velocity.baseValue = config_.rotate.velocity.baseValue;
			if (config_.rotate.acceleration.isRandom) r.acceleration.baseValue = RandInAABB(config_.rotate.acceleration.randomRange_min, config_.rotate.acceleration.randomRange_max);
			else r.acceleration.baseValue = config_.rotate.acceleration.baseValue;

			if (config_.scale.value.isRandom) s.value.baseValue = RandInAABB(config_.scale.value.randomRange_min, config_.scale.value.randomRange_max);
			else s.value.baseValue = config_.scale.value.baseValue;
			if (config_.scale.velocity.isRandom) s.velocity.baseValue = RandInAABB(config_.scale.velocity.randomRange_min, config_.scale.velocity.randomRange_max);
			else s.velocity.baseValue = config_.scale.velocity.baseValue;
			if (config_.scale.acceleration.isRandom) s.acceleration.baseValue = RandInAABB(config_.scale.acceleration.randomRange_min, config_.scale.acceleration.randomRange_max);
			else s.acceleration.baseValue = config_.scale.acceleration.baseValue;

			instances_.push_back({ s,r,t,0.0f, Vector4(1,1,1,1) });
		}
	}

	emitTimer_ += dt;

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
			[this](const ParticleInstance& p) { return p.scale.value.baseValue.x < 0.0f || p.scale.value.baseValue.y < 0.0f || p.scale.value.baseValue.z < 0.0f; }),
		instances_.end());


	// SRT更新 & GPU転送データ作成
	for (auto& instance : instances_)
	{
		instance.translate.value.baseValue += instance.translate.velocity.baseValue * dt + 0.5f * instance.translate.acceleration.baseValue * dt * dt;
		instance.translate.velocity.baseValue += instance.translate.acceleration.baseValue * dt;
		instance.rotate.value.baseValue += instance.rotate.velocity.baseValue * dt + 0.5f * instance.rotate.acceleration.baseValue * dt * dt;
		instance.rotate.velocity.baseValue += instance.rotate.acceleration.baseValue * dt;
		instance.scale.value.baseValue += instance.scale.velocity.baseValue * dt + 0.5f * instance.scale.acceleration.baseValue * dt * dt;
		instance.scale.velocity.baseValue += instance.scale.acceleration.baseValue * dt;

		const Matrix4x4 world = Matrix::MakeAffineMatrix(instance.scale.value.baseValue, instance.rotate.value.baseValue, instance.translate.value.baseValue) * modelWorld_;

		particle_.pushInstance(world, instance.color);
	}
}


void PhysicsParticle::SetEnabled(bool isActive)
{
	isActive_ = isActive;
	if (isActive_)
	{
		emitTimer_ = config_.cfg.emitInterval;

	}
	else 
	{
		emitTimer_ = 0.0f;
	}
}
