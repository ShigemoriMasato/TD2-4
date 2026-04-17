#pragma once
#include <Scene/iscene.h>
#include <Render/Font/Text.h>
#include <Camera/DebugCamera.h>
#include <Render/Renderer.h>
#include <Compute/ComputeObject.h>

struct EmitConfig {
	int emitCount = 1;
	int isEmit = true;
};

struct InitConfig {
	Vector3 position = {};
	float padding;
	Vector3 velocity = { 0.0f, 1.0f, 0.0f };
};

class TechnicalTestScene : public IScene {
public:

	virtual void Initialize() override;
	virtual std::unique_ptr<IScene> Update() override;
	virtual void Draw() override;

private:

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
	float deadTime_ = 1.0f;
};
