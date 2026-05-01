#include "Particle.h"
#include <algorithm>
#include <Utility/MatrixFactory.h>


void Particle::Initialize(
	SHEngine::TextureManager* textureManager,
	SHEngine::ModelManager* modelManager)
{
	textureManager_ = textureManager;
	modelManager_ = modelManager;

	gpuInstances_.resize(kMaxParticles_);
	Clear();
}

void Particle::SetModel(const std::string& modelPath)
{
	modelHandle_ = modelManager_->LoadModel(modelPath);
}

void Particle::SetTexture(const std::string& texturePath)
{
	textureHandle_ = textureManager_->LoadTexture(texturePath);
}

void Particle::Clear()
{
	// 履歴クリア
	aliveCount_ = 0;
	// GPUに送るようの情報を初期化(重い)
	std::fill(gpuInstances_.begin(), gpuInstances_.end(), InstanceGpu{});
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

void Particle::pushInstance(const Matrix4x4& world, const Vector4& color)
{
	if (aliveCount_ >= kMaxParticles_) return;

	InstanceGpu temp{};
	temp.world = world;
	temp.color = color;
	temp.textureIndex = static_cast<uint32_t>(textureHandle_);
	temp.modelIndex = static_cast<uint32_t>(modelHandle_);
	gpuInstances_[aliveCount_++] = temp;
}