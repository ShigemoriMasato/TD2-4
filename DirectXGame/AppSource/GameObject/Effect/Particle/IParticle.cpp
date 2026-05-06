#include "IParticle.h"
#include <algorithm>

void IParticle::Initialize(SHEngine::TextureManager* textureManager, SHEngine::ModelManager* modelManager)
{
	textureManager_ = textureManager;
	modelManager_ = modelManager;

	gpuInstances_.resize(kMaxParticles_);
	Clear();
}

void IParticle::Clear()
{
	// 履歴クリア
	aliveCount_ = 0;
	// GPUに送るようの情報を初期化(重い)
	std::fill(gpuInstances_.begin(), gpuInstances_.end(), InstanceGpu{});
}

void IParticle::SetEnabled(bool isActive)
{
	isActive_ = isActive;
	emitTimer_ = -1.0f;
}

std::vector<Matrix4x4> IParticle::GetParticleWorlds() const
{
	std::vector<Matrix4x4> worlds;
	worlds.resize(aliveCount_);
	for (size_t i = 0; i < aliveCount_; ++i)
	{
		worlds[i] = gpuInstances_[i].world;
	}
	return worlds;
}

void IParticle::pushInstance(const Matrix4x4& world, const Vector4& color)
{
	if (aliveCount_ >= kMaxParticles_) return;

	InstanceGpu temp{};
	temp.world = world;
	temp.color = color;
	temp.textureIndex = static_cast<uint32_t>(textureHandle_);
	temp.modelIndex = static_cast<uint32_t>(modelHandle_);
	gpuInstances_[aliveCount_++] = temp;
}
