#include "Particle.h"
#include <algorithm>
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

void Particle::Initialize(
	SHEngine::TextureManager* textureManager,
	SHEngine::ModelManager* modelManager)
{
	textureManager_ = textureManager;
	modelManager_ = modelManager;

	gpuInstances_.resize(kMaxParticles_);
	Clear();
}

void Particle::SetConfig(const Config& config)
{
	config_ = config;

	modelHandle_ = modelManager_->LoadModel(config_.modelPath);
	textureHandle_ = textureManager_->LoadTexture(config_.texturePath);
}

void Particle::Clear()
{
	instances_.clear();
	emitting_ = false;
	emitTimer_ = 0.0f;
	emitPos_ = {};
	aliveCount_ = 0;
}

std::vector<Matrix4x4> Particle::GetParticleWorlds() const
{
	std::vector<Matrix4x4> worlds;
	worlds.resize(aliveCount_);
	for (size_t i = 0; i < aliveCount_; ++i)
	{
		worlds[i] = gpuInstances_[i].world;
	}
	return worlds;
}

void Particle::Emit(const Vector3& pos)
{
	for (int i = 0; i < config_.emitNum; ++i)
	{
		if (instances_.size() >= kMaxParticles_) break;
		ParticleInstance temp{};

		if (config_.translate.isRandom_value) temp.translate.value = RandInAABB(config_.translate.randomRange_value_min, config_.translate.randomRange_value_max);
		else temp.translate.value = config_.translate.initial.value;

		if (config_.translate.isRandom_velocity) temp.translate.velocity = RandInAABB(config_.translate.randomRange_velocity_min, config_.translate.randomRange_velocity_max);
		else temp.translate.velocity = config_.translate.initial.velocity;

		if (config_.translate.isRandom_acceleration) temp.translate.acceleration = RandInAABB(config_.translate.randomRange_acceleration_min, config_.translate.randomRange_acceleration_max);
		else temp.translate.acceleration = config_.translate.initial.acceleration;

		if (config_.rotate.isRandom_value) temp.rotate.value = RandInAABB(config_.rotate.randomRange_value_min, config_.rotate.randomRange_value_max);
		else temp.rotate.value = config_.rotate.initial.value;

		if (config_.rotate.isRandom_velocity) temp.rotate.velocity = RandInAABB(config_.rotate.randomRange_velocity_min, config_.rotate.randomRange_velocity_max);
		else temp.rotate.velocity = config_.rotate.initial.velocity;

		if (config_.rotate.isRandom_acceleration) temp.rotate.acceleration = RandInAABB(config_.rotate.randomRange_acceleration_min, config_.rotate.randomRange_acceleration_max);
		else temp.rotate.acceleration = config_.rotate.initial.acceleration;

		if (config_.scale.isRandom_value) temp.scale.value = RandInAABB(config_.scale.randomRange_value_min, config_.scale.randomRange_value_max);
		else temp.scale.value = config_.scale.initial.value;

		if (config_.scale.isRandom_velocity) temp.scale.velocity = RandInAABB(config_.scale.randomRange_velocity_min, config_.scale.randomRange_velocity_max);
		else temp.scale.velocity = config_.scale.initial.velocity;

		if (config_.scale.isRandom_acceleration) temp.scale.acceleration = RandInAABB(config_.scale.randomRange_acceleration_min, config_.scale.randomRange_acceleration_max);
		else temp.scale.acceleration = config_.scale.initial.acceleration;

		temp.age = 0.0f;
		temp.translate.value += pos;
		instances_.push_back(temp);
	}
}

void Particle::Update(float deltaTime)
{
	for (auto& ins : instances_)
	{
		ins.age += deltaTime;

		ins.translate.velocity += ins.translate.acceleration * deltaTime;
		ins.rotate.velocity += ins.rotate.acceleration * deltaTime;
		ins.scale.velocity += ins.scale.acceleration * deltaTime;

		ins.translate.value += ins.translate.velocity * deltaTime * config_.speed;
		ins.rotate.value += ins.rotate.velocity * deltaTime * config_.speed;
		ins.scale.value += ins.scale.velocity * deltaTime * config_.speed;
	}

	instances_.erase(
		std::remove_if(instances_.begin(), instances_.end(),
			[this](const ParticleInstance& p) { return p.age >= config_.lifeTime; }),
		instances_.end());

	instances_.erase(
		std::remove_if(instances_.begin(), instances_.end(),
			[](const ParticleInstance& p)
			{
				return p.scale.value.x <= 0.0f || p.scale.value.y <= 0.0f || p.scale.value.z <= 0.0f;
			}),
		instances_.end());

	if (emitting_)
	{
		emitTimer_ += deltaTime;
		while (emitTimer_ >= config_.emitInterval)
		{
			emitTimer_ -= config_.emitInterval;
			Emit(emitPos_);
		}
	}

	aliveCount_ = std::min(instances_.size(), static_cast<size_t>(kMaxParticles_));

	for (uint32_t i = 0; i < aliveCount_; ++i)
	{
		const Matrix4x4 world = Matrix::MakeAffineMatrix(
			instances_[i].scale.value,
			instances_[i].rotate.value,
			instances_[i].translate.value);

		gpuInstances_[i].world = world;
		gpuInstances_[i].textureIndex = static_cast<uint32_t>(textureHandle_);
		gpuInstances_[i].color = Vector4(1, 1, 1, 1);
	}
}