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
		VectorDynamics initial; // random生成されないときの初期値

		bool isRandom_value = false; // trueならランダム生成
		Vector3 randomRange_value_min;
		Vector3 randomRange_value_max;

		bool isRandom_velocity = false; // trueならランダム生成
		Vector3 randomRange_velocity_min;
		Vector3 randomRange_velocity_max;

		bool isRandom_acceleration = false; // trueならランダム生成
		Vector3 randomRange_acceleration_min;
		Vector3 randomRange_acceleration_max;
	};


	struct Config
	{
		float lifeTime = 1.0f;    // 寿命
		float speed = 1.0f;       // 速度

		// emitInterval秒ごとにemitNum個生成する
		int emitNum = 10; 
		float emitInterval = 0.1f;
		
		// 物理挙動
		ParticleSRT scale;
		ParticleSRT rotate;
		ParticleSRT translate;

		// 見た目
		std::string texturePath = "Assets/.EngineResource/Texture/white1x1.png";
		std::string modelPath = "Assets/.EngineResource/Model/Cube";
	};

public:

	Particle() = default;
	~Particle() = default;

	void Initialize(
		SHEngine::DrawDataManager* drawDataManager,
		SHEngine::TextureManager* textureManager,
		SHEngine::ModelManager* modelManager,
		const Config& config = {});
	void Update(float deltaTime, const Matrix4x4& vpMatrix);
	void Draw(CmdObj* cmdObj);
	void Trigger(const Vector3& pos);
	void Stop();

	void SetConfig(const Config& config);
	const Config& GetConfig() const { return config_; }

	void Clear();

private:
	struct ParticleInstance
	{
		VectorDynamics scale;
		VectorDynamics rotate;
		VectorDynamics translate;
		float age = 0.0f;
	};

	void EnsureRender();
	void Emit(const Vector3& pos);

	SHEngine::DrawDataManager* drawDataManager_ = nullptr;
	SHEngine::TextureManager* textureManager_ = nullptr;
	SHEngine::ModelManager* modelManager_ = nullptr;

	// 設定
	Config config_{};
	// 初期値
	VectorDynamics scale;
	VectorDynamics rotate;
	VectorDynamics translate;

	// 発生するか否かフラグ
	bool emitting_ = false;
	// 発生タイマー
	float emitTimer_ = 0.0f;
	// 発生位置
	Vector3 emitPos_{};
	// 生存しているパーティクルの数
	size_t aliveCount_ = 0;

	std::vector<ParticleInstance> instances_;

	// 描画
	std::unique_ptr<SHEngine::RenderObject> render_;
	int modelHandle_ = -1;
	int textureHandle_ = -1;

	// GPUに送る用のインスタンスデータ
	std::vector<Matrix4x4> gpuInstances_;
};