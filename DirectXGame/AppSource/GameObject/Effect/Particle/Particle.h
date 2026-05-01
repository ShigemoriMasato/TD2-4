#pragma once
#include <memory>
#include <Utility/Matrix.h>
#include <Render/DrawDataManager.h>
#include <Assets/Texture/TextureManager.h>
#include <Assets/Model/ModelManager.h>
#include <GameObject/Random/Random.h>

class Particle
{
public:
	static constexpr uint32_t kMaxParticles_ = 4096;

	struct Config
	{
		float lifeTime = 1.0f;
		float speed = 1.0f;

		int emitNum = 10;
		float emitInterval = 0.1f;

		std::string texturePath = "Assets/.EngineResource/Texture/white1x1.png";
		std::string modelPath = "Assets/.EngineResource/Model/Cube";
	};

	struct InstanceGpu
	{
		Matrix4x4 world{ Matrix4x4::Identity() };
		Vector4 color{ 1,1,1,1 };
		uint32_t textureIndex = 0;
		uint32_t modelIndex = 0;
	};

public:
	Particle() = default;
	~Particle() = default;

	void Initialize(SHEngine::TextureManager* textureManager, SHEngine::ModelManager* modelManager);

	// 制御
	void Clear();

	// テクスチャ差し替え
	void SetTexture(const std::string& texturePath);
	// モデル差し替え
	void SetModel(const std::string& modelPath);

	// Drawer用
	int GetModelHandle() const { return modelHandle_; }
	int GetTextureHandle() const { return textureHandle_; }
	const InstanceGpu* GetGpuInstanceData() const { return gpuInstances_.data(); }
	uint32_t GetGpuInstanceCount() const { return static_cast<uint32_t>(aliveCount_); }

	std::vector<Matrix4x4> GetParticleWorlds() const;
	size_t GetAliveCount() const { return aliveCount_; }

	void pushInstance(const Matrix4x4& world, const Vector4& color = { 1,1,1,1 });

private:
	// 外部
	SHEngine::TextureManager* textureManager_ = nullptr;
	SHEngine::ModelManager* modelManager_ = nullptr;

	// 履歴
	size_t aliveCount_ = 0;


	// GPU転送用
	std::vector<InstanceGpu> gpuInstances_;

	int modelHandle_ = -1;
	int textureHandle_ = -1;
};