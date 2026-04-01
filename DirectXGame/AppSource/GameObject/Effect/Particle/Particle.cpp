#include "Particle.h"
#include <algorithm>
#include <random>

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
	SHEngine::DrawDataManager* drawDataManager,
	SHEngine::TextureManager* textureManager,
	SHEngine::ModelManager* modelManager,
	const Config& config)
{
	drawDataManager_ = drawDataManager;
	textureManager_ = textureManager;
	modelManager_ = modelManager;

	gpuInstances_.resize(kMaxParticles_);

	// renderObjectのインスタンス作成
	EnsureRender();
	// configセット&モデルとテクスチャの読み込み
	SetConfig(config);
	// パーティクルインスタンス配列をクリア
	Clear();
}

// renderObjectのインスタンス作成
void Particle::EnsureRender()
{
	render_ = std::make_unique<SHEngine::RenderObject>("Particle");
	render_->Initialize();

	render_->psoConfig_.vs = "Particle/Particle.VS.hlsl";
	render_->psoConfig_.ps = "Particle/Particle.PS.hlsl";
	render_->SetUseTexture(true);

	render_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "VPMatrix");
	render_->CreateSRV(sizeof(Matrix4x4), kMaxParticles_, ShaderType::VERTEX_SHADER, "ParticleInstances(vector)");
	render_->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "ParticleColor");
	render_->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "ParticleTextureIndex");
}

// configセット&モデルとテクスチャの読み込み
void Particle::SetConfig(const Config& config)
{
	config_ = config;

	modelHandle_ = modelManager_->LoadModel(config_.modelPath);
	auto modelData = modelManager_->GetNodeModelData(modelHandle_);
	auto drawData = drawDataManager_->GetDrawData(modelData.drawDataIndex);
	render_->SetDrawData(drawData);

	textureHandle_ = textureManager_->LoadTexture(config_.texturePath);
}

// パーティクルインスタンス配列をクリア
void Particle::Clear()
{
	// パーティクルインスタンス配列をクリア
	instances_.clear();
	// 発生フラグ折る
	emitting_ = false;
	// タイマーリセット
	emitTimer_ = 0.0f;
	// 発生位置リセット
	emitPos_ = {};
}

void Particle::Trigger(const Vector3& pos)
{
	emitting_ = true;
	emitPos_ = pos;

	Emit(pos);
}

void Particle::Stop()
{
	emitting_ = false;
}

std::vector<Matrix4x4> Particle::GetParticleWorlds() const
{
	return gpuInstances_;
}

// config_.emitNum個分パーティクルを生成
void Particle::Emit(const Vector3& pos)
{
	for (int i = 0; i < config_.emitNum; ++i)
	{
		if (instances_.size() >= kMaxParticles_) break;
		ParticleInstance temp{};

		if (config_.translate.isRandom_value)
		{
			temp.translate.value = RandInAABB(config_.translate.randomRange_value_min, config_.translate.randomRange_value_max);
		}
		else
		{
			temp.translate.value = config_.translate.initial.value;
		}
		if (config_.translate.isRandom_velocity)
		{
			temp.translate.velocity = RandInAABB(config_.translate.randomRange_velocity_min, config_.translate.randomRange_velocity_max);
		}
		else
		{
			temp.translate.velocity = config_.translate.initial.velocity;
		}
		if (config_.translate.isRandom_acceleration)
		{
			temp.translate.acceleration = RandInAABB(config_.translate.randomRange_acceleration_min, config_.translate.randomRange_acceleration_max);
		}
		else
		{
			temp.translate.acceleration = config_.translate.initial.acceleration;
		}

		if (config_.rotate.isRandom_value)
		{
			temp.rotate.value = RandInAABB(config_.rotate.randomRange_value_min, config_.rotate.randomRange_value_max);
		}
		else
		{
			temp.rotate.value = config_.rotate.initial.value;
		}
		if (config_.rotate.isRandom_velocity)
		{
			temp.rotate.velocity = RandInAABB(config_.rotate.randomRange_velocity_min, config_.rotate.randomRange_velocity_max);
		}
		else
		{
			temp.rotate.velocity = config_.rotate.initial.velocity;
		}
		if (config_.rotate.isRandom_acceleration)
		{
			temp.rotate.acceleration = RandInAABB(config_.rotate.randomRange_acceleration_min, config_.rotate.randomRange_acceleration_max);
		}
		else
		{
			temp.rotate.acceleration = config_.rotate.initial.acceleration;
		}

		if (config_.scale.isRandom_value)
		{
			temp.scale.value = RandInAABB(config_.scale.randomRange_value_min, config_.scale.randomRange_value_max);
		}
		else
		{
			temp.scale.value = config_.scale.initial.value;
		}
		if (config_.scale.isRandom_velocity)
		{
			temp.scale.velocity = RandInAABB(config_.scale.randomRange_velocity_min, config_.scale.randomRange_velocity_max);
		}
		else
		{
			temp.scale.velocity = config_.scale.initial.velocity;
		}
		if (config_.scale.isRandom_acceleration)
		{
			temp.scale.acceleration = RandInAABB(config_.scale.randomRange_acceleration_min, config_.scale.randomRange_acceleration_max);
		}
		else
		{
			temp.scale.acceleration = config_.scale.initial.acceleration;
		}

		temp.age = 0.0f;
		instances_.push_back(temp);
	}
}

void Particle::Update(float deltaTime, const Matrix4x4& vpMatrix)
{
	// 寿命更新 & 物理演算
	for (auto& ins : instances_)
	{
		ins.age += deltaTime;

		ins.translate.velocity += ins.translate.acceleration * deltaTime;
		ins.rotate.velocity += ins.rotate.acceleration * deltaTime;
		ins.scale.velocity += ins.scale.acceleration * deltaTime;

		ins.translate.value += ins.translate.velocity * deltaTime * config_.speed;
		ins.rotate.value += ins.rotate.velocity * deltaTime * config_.speed;
		ins.scale.value += ins.scale.velocity * deltaTime * config_.speed;

		// 一旦すべてのParticleはscale0になったら消滅
		if (ins.scale.value.x <= 0.0f || ins.scale.value.y <= 0.0f || ins.scale.value.z <= 0.0f)
		{
			ins.age = config_.lifeTime;
		}
	}

	// 寿命で削除
	instances_.erase(
		std::remove_if(instances_.begin(), instances_.end(),
			[this](const ParticleInstance& p) { return p.age >= config_.lifeTime; }),
		instances_.end());

	// 発生
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

		gpuInstances_[i] = world;
	}

	const Vector4 color = { 1, 1, 1, 1 };

	render_->CopyBufferData(0, &vpMatrix, sizeof(Matrix4x4));
	render_->CopyBufferData(1, gpuInstances_.data(), sizeof(Matrix4x4) * aliveCount_);
	render_->CopyBufferData(2, &color, sizeof(Vector4));
	render_->CopyBufferData(3, &textureHandle_, sizeof(int));
}

void Particle::Draw(CmdObj* cmdObj)
{
	if (!render_) return;

	if (aliveCount_ == 0) { return; }


	render_->instanceNum_ = uint32_t(aliveCount_);
	render_->Draw(cmdObj);
}