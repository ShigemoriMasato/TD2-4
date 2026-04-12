#pragma once
#include <memory>
#include <Utility/Matrix.h>
#include <Render/DrawDataManager.h>
#include <Assets/Texture/TextureManager.h>
#include <Assets/Model/ModelManager.h>
#include <Render/RenderObject.h>
#include <GameObject/Random/Random.h>

class Particle
{
public:
	static constexpr uint32_t kMaxParticles_ = 4096;

	struct VectorDynamics
	{
		Vector3 value = { 1.0f,1.0f,1.0f };
		Vector3 velocity;
		Vector3 acceleration;
	};

	struct ParticleSRT
	{
		VectorDynamics initial;

		bool isRandom_value = false;
		Vector3 randomRange_value_min;
		Vector3 randomRange_value_max;

		bool isRandom_velocity = false;
		Vector3 randomRange_velocity_min;
		Vector3 randomRange_velocity_max;

		bool isRandom_acceleration = false;
		Vector3 randomRange_acceleration_min;
		Vector3 randomRange_acceleration_max;
	};

	struct Config
	{
		float lifeTime = 1.0f;
		float speed = 1.0f;

		int emitNum = 10;
		float emitInterval = 0.1f;

		ParticleSRT scale;
		ParticleSRT rotate;
		ParticleSRT translate;

		bool isMoveToTarget = false;
		Vector3 TargetPos = { 0.0f, 0.0f, 0.0f };
		float moveSpeed = 1.0f;

		std::string texturePath = "Assets/.EngineResource/Texture/white1x1.png";
		std::string modelPath = "Assets/.EngineResource/Model/Cube";
	};

	struct InstanceGpu
	{
		Matrix4x4 world{ Matrix4x4::Identity() };
		uint32_t textureIndex = 0;
		Vector4 color{ 1,1,1,1 };
	};

public:
	Particle() = default;
	~Particle() = default;

	void Initialize(
		SHEngine::DrawDataManager* drawDataManager,
		SHEngine::TextureManager* textureManager,
		SHEngine::ModelManager* modelManager);

	void SetConfig(const Config& config);
	void Update(float deltaTime, const Matrix4x4& vpMatrix);
	void Draw(CmdObj* cmdObj);

	void SetEmitPos(const Vector3& pos) { emitPos_ = pos; }
	void SetEmittingFlag(bool flag) { emitting_ = flag; }

	std::vector<Matrix4x4> GetParticleWorlds() const;
	size_t GetAliveCount() const { return aliveCount_; }

	const Config& GetConfig() const { return config_; }

	void Clear();

	// Drawer用（コピー無し）
	int GetModelHandle() const { return modelHandle_; }
	int GetTextureHandle() const { return textureHandle_; }
	const InstanceGpu* GetGpuInstanceData() const { return gpuInstances_.data(); }
	uint32_t GetGpuInstanceCount() const { return static_cast<uint32_t>(aliveCount_); }

private:
	struct ParticleInstance
	{
		VectorDynamics scale;
		VectorDynamics rotate;
		VectorDynamics translate;
		float age = 0.0f;
		uint32_t id = 0;
	};

	void EnsureRender();
	void Emit(const Vector3& pos);

private:
	SHEngine::DrawDataManager* drawDataManager_ = nullptr;
	SHEngine::TextureManager* textureManager_ = nullptr;
	SHEngine::ModelManager* modelManager_ = nullptr;

	Config config_{};

	VectorDynamics scale;
	VectorDynamics rotate;
	VectorDynamics translate;

	bool emitting_ = false;
	float emitTimer_ = 0.0f;
	Vector3 emitPos_{};
	size_t aliveCount_ = 0;

	std::vector<ParticleInstance> instances_;

	std::unique_ptr<SHEngine::RenderObject> render_;
	int modelHandle_ = -1;
	int textureHandle_ = -1;

	// GPUに送る用のインスタンスデータ（固定長）
	std::vector<InstanceGpu> gpuInstances_;
};