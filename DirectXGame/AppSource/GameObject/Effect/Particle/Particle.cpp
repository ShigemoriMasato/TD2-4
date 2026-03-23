#include "Particle.h"
#include <algorithm>
#include <random>

namespace
{
	float Rand01()
	{
		static thread_local std::mt19937 rng{ std::random_device{}() };
		static thread_local std::uniform_real_distribution<float> dist(0.0f, 1.0f);
		return dist(rng);
	}

	Vector3 RandInAABB(const Vector3& min, const Vector3& max)
	{
		return Vector3(
			min.x + (max.x - min.x) * Rand01(),
			min.y + (max.y - min.y) * Rand01(),
			min.z + (max.z - min.z) * Rand01()
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

	SetConfig(config);
	EnsureRender_();
	Clear();
}

void Particle::SetConfig(const Config& config)
{
	config_ = config;

	config_.lifeTime = std::max(0.001f, config_.lifeTime);
	config_.speed = std::max(0.0f, config_.speed);
	config_.emitNum = std::max(0, config_.emitNum);
	config_.emitInterval = std::max(0.0f, config_.emitInterval);
}

void Particle::EnsureRender_()
{
	render_ = std::make_unique<SHEngine::RenderObject>("Particle");
	render_->Initialize();

	render_->psoConfig_.vs = "Game/Particle.VS.hlsl";
	render_->psoConfig_.ps = "Game/Particle.PS.hlsl";
	render_->SetUseTexture(true);

	render_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER);
	render_->CreateSRV(sizeof(Matrix4x4), kMaxParticles_, ShaderType::VERTEX_SHADER, "ParticleInstances");
	render_->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "Color");
	render_->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");

	modelHandle_ = modelManager_->LoadModel(config_.modelPath);
	auto modelData = modelManager_->GetNodeModelData(modelHandle_);
	auto drawData = drawDataManager_->GetDrawData(modelData.drawDataIndex);
	render_->SetDrawData(drawData);

	textureHandle_ = textureManager_->LoadTexture(config_.texturePath);
}

void Particle::Clear()
{
	instances_.clear();
	emitting_ = false;
	emitTimer_ = 0.0f;
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

void Particle::Emit(const Vector3& pos)
{
	for (int i = 0; i < config_.emitNum; ++i)
	{
		if (instances_.size() >= kMaxParticles_) break;
		ParticleInstance temp{};
		temp.translate.value = pos + RandInAABB(config_.emitterMin, config_.emitterMax);
		temp.scale.value = config_.scale.value;
		temp.rotate.value = config_.rotate.value;
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
	}

	// 寿命で削除
	instances_.erase(
		std::remove_if(instances_.begin(), instances_.end(),
			[this](const ParticleInstance& p) { return p.age >= config_.lifeTime; }),
		instances_.end());

	// 連続発生
	if (emitting_ && config_.emitInterval > 0.0f)
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