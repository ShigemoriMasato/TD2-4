#include "PhysicsParticle.h"
#include <Utility/MatrixFactory.h>
#include <GameObject/Random/Random.h>

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

void PhysicsParticle::SetConfig(const ParticlePresetVariant& config)
{
	config_ = config;

	const PhysicsConfig& uniqueConfig = std::get<PhysicsConfig>(config_);
	SetModel(uniqueConfig.cfg.modelPath);
	SetTexture(uniqueConfig.cfg.texturePath);
}

void PhysicsParticle::Update(float dt)
{
	Clear();

	// 「いま生きてる粒が無い」かつ「emitも止まってる」なら何もしない
	if (!isActive_ && instances_.empty()) return;

	const PhysicsConfig& uniqueConfig = std::get<PhysicsConfig>(config_);

	// 一定時間ごとにパーティクルを発生させる
	if (isActive_ && (emitTimer_ >= uniqueConfig.cfg.emitInterval || emitTimer_ < 0.0f))
	{
		emitTimer_ = 0.0f;

		for (int32_t i = 0; i < uniqueConfig.cfg.emitNum; ++i)
		{
			ParticleSRT s;
			ParticleSRT r;
			ParticleSRT t;

			if (uniqueConfig.translate.value.isRandom) t.value.baseValue = RandInAABB(uniqueConfig.translate.value.randomRange_min, uniqueConfig.translate.value.randomRange_max);
			else t.value.baseValue = uniqueConfig.translate.value.baseValue;
			if (uniqueConfig.translate.velocity.isRandom) t.velocity.baseValue = RandInAABB(uniqueConfig.translate.velocity.randomRange_min, uniqueConfig.translate.velocity.randomRange_max);
			else t.velocity.baseValue = uniqueConfig.translate.velocity.baseValue;
			if (uniqueConfig.translate.acceleration.isRandom) t.acceleration.baseValue = RandInAABB(uniqueConfig.translate.acceleration.randomRange_min, uniqueConfig.translate.acceleration.randomRange_max);
			else t.acceleration.baseValue = uniqueConfig.translate.acceleration.baseValue;

			if (uniqueConfig.rotate.value.isRandom) r.value.baseValue = RandInAABB(uniqueConfig.rotate.value.randomRange_min, uniqueConfig.rotate.value.randomRange_max);
			else r.value.baseValue = uniqueConfig.rotate.value.baseValue;
			if (uniqueConfig.rotate.velocity.isRandom) r.velocity.baseValue = RandInAABB(uniqueConfig.rotate.velocity.randomRange_min, uniqueConfig.rotate.velocity.randomRange_max);
			else r.velocity.baseValue = uniqueConfig.rotate.velocity.baseValue;
			if (uniqueConfig.rotate.acceleration.isRandom) r.acceleration.baseValue = RandInAABB(uniqueConfig.rotate.acceleration.randomRange_min, uniqueConfig.rotate.acceleration.randomRange_max);
			else r.acceleration.baseValue = uniqueConfig.rotate.acceleration.baseValue;

			if (uniqueConfig.scale.value.isRandom) s.value.baseValue = RandInAABB(uniqueConfig.scale.value.randomRange_min, uniqueConfig.scale.value.randomRange_max);
			else s.value.baseValue = uniqueConfig.scale.value.baseValue;
			if (uniqueConfig.scale.velocity.isRandom) s.velocity.baseValue = RandInAABB(uniqueConfig.scale.velocity.randomRange_min, uniqueConfig.scale.velocity.randomRange_max);
			else s.velocity.baseValue = uniqueConfig.scale.velocity.baseValue;
			if (uniqueConfig.scale.acceleration.isRandom) s.acceleration.baseValue = RandInAABB(uniqueConfig.scale.acceleration.randomRange_min, uniqueConfig.scale.acceleration.randomRange_max);
			else s.acceleration.baseValue = uniqueConfig.scale.acceleration.baseValue;

			instances_.push_back({ s,r,t,0.0f, Vector4(1,1,1,1) });
		}
	}

	emitTimer_ += dt;

	// 年齢更新 & SRT更新
	for (auto& instance : instances_)
	{
		instance.translate.value.baseValue += instance.translate.velocity.baseValue * dt + 0.5f * instance.translate.acceleration.baseValue * dt * dt;
		instance.translate.velocity.baseValue += instance.translate.acceleration.baseValue * dt;
		instance.rotate.value.baseValue += instance.rotate.velocity.baseValue * dt + 0.5f * instance.rotate.acceleration.baseValue * dt * dt;
		instance.rotate.velocity.baseValue += instance.rotate.acceleration.baseValue * dt;
		instance.scale.value.baseValue += instance.scale.velocity.baseValue * dt + 0.5f * instance.scale.acceleration.baseValue * dt * dt;
		instance.scale.velocity.baseValue += instance.scale.acceleration.baseValue * dt;

		instance.age += dt;
	}

	// 寿命切れ削除 & スケールがマイナスのものも削除
	instances_.erase(
		std::remove_if(instances_.begin(), instances_.end(),
			[&uniqueConfig](const ParticleInstance& p)
			{ 
				return
					p.age >= uniqueConfig.cfg.lifeTime || 
					p.scale.value.baseValue.x < 0.0f || 
					p.scale.value.baseValue.y < 0.0f || 
					p.scale.value.baseValue.z < 0.0f;
			}),
		instances_.end());

	// GPU転送データ作成
	for (auto& instance : instances_)
	{
		const Matrix4x4 world = Matrix::MakeAffineMatrix(instance.scale.value.baseValue, instance.rotate.value.baseValue, instance.translate.value.baseValue) * modelWorld_;

		pushInstance(world, instance.color);
	}
}
