#pragma once
#include <memory>
#include <Utility/Matrix.h>
#include <Render/DrawDataManager.h>
#include <Assets/Texture/TextureManager.h>
#include <Assets/Model/ModelManager.h>
#include "GameObject/Effect/Particle/Preset/ParticlePreset.h"

class Particle
{
public:

	struct VectorDynamics
	{
		Vector3 value = { 1.0f,1.0f,1.0f };
		Vector3 velocity;
		Vector3 acceleration;
	};

	struct Config
	{
		float lifeTime = 1.0f;    // 寿命
		float speed = 1.0f;       // 速度

		// emitInterval秒ごとにemitNum個生成する
		int emitNum = 10; 
		float emitInterval = 0.1f;
		
		
		VectorDynamics scale;
		VectorDynamics rotate;
		VectorDynamics translate;

		Vector3 emitterMin;
		Vector3 emitterMax;

		std::string texturePath = "Assets/.EngineResource/Texture/white1x1.png";
		std::string modelPath = "Assets/.EngineResource/Model/Cube";
	};

public:

	Particle() = default;
	~Particle() = default;

	// 種類選択 所詮enumなのでintを渡したってかまわない
	void SetType(ParticleType type);

	void Initialize(
		SHEngine::DrawDataManager* drawDataManager,
		SHEngine::TextureManager* textureManager,
		SHEngine::ModelManager* modelManager,
		const Config& config = {});
	void Update(float deltaTime);
	void Draw(CmdObj* cmdObj, const Matrix4x4& vpMatrix);
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

	void EnsureRender_();
	void Emit_(const Vector3& pos);

	SHEngine::DrawDataManager* drawDataManager_ = nullptr;
	SHEngine::TextureManager* textureManager_ = nullptr;
	SHEngine::ModelManager* modelManager_ = nullptr;

	Config config_{};

	bool emitting_ = false;
	float emitTimer_ = 0.0f;
	Vector3 emitPos_{};

	std::vector<ParticleInstance> instances_;

	// 描画
	std::unique_ptr<SHEngine::RenderObject> render_;
	int modelHandle_ = -1;
	int textureHandle_ = -1;
};