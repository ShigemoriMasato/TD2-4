#pragma once
#include <Scene/iscene.h>
#include <Render/Font/Text.h>
#include <Camera/DebugCamera.h>
#include <Render/Renderer.h>
#include <Compute/ComputeObject.h>
#include <random>

struct EmitConfig {
	int emitCount = 1;
	int isEmit = true;
};

struct InitConfig {
	Vector3 position = {};
	float lifeTime = 1.0f;
	Vector3 velocity = { 0.0f, 1.0f, 0.0f };
};

class TechnicalTestScene : public IScene {
public:

	virtual void Initialize() override;
	virtual std::unique_ptr<IScene> Update() override;
	virtual void Draw() override;

private:

	std::unique_ptr<SHEngine::Command::Object> computeCmdObj_ = nullptr;
	std::unique_ptr<SHEngine::Command::Object> particleCmdObj_ = nullptr;
	SHEngine::Command::WaitFence particleLastFence_ = {};
	std::unique_ptr<DebugCamera> debugCamera_ = nullptr;

	std::unique_ptr<SHEngine::Text> text_ = nullptr;
	Transform textTransform_ = {};

	std::unique_ptr<SHEngine::ComputeObject> particleInit_ = nullptr;
	std::unique_ptr<SHEngine::ComputeObject> particleEmit_ = nullptr;
	std::unique_ptr<SHEngine::ComputeObject> particleUpdate_ = nullptr;
	std::unique_ptr<SHEngine::Renderer> renderer_ = nullptr;
	SHEngine::BufferContainer bufferCont_;

	EmitConfig emitConfig_ = {};
	InitConfig initConfig_ = {};
	float size_ = 0.2f;

	std::mt19937 randomEngine_{ std::random_device{}() };
	std::uniform_real_distribution<float> velDistX_{ -1.0f, 1.0f };
	std::uniform_real_distribution<float> velDistY_{ -1.0f, 1.0f };
	std::uniform_real_distribution<float> velDistZ_{ -1.0f, 1.0f };
};
