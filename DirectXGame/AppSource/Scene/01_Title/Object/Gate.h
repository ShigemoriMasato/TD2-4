#pragma once
#include <SHEngine.h>
#include <Render/RenderObject.h>
#include <Camera/Camera.h>
#include <Scene/01_Title/Light/LightManager.h>

class Gate {
public:

	~Gate();

	void Initialize(SHEngine::Engine* engine, const LightData& lightData);
	void Update(float deltaTime, Camera* camera);
	void Draw(CmdObj* cmdObj);

	void Open();
	void Reset();

private:

	void Save();
	void Load();

	std::unique_ptr<SHEngine::BufferContainer> container_;

	std::unique_ptr<SHEngine::Renderer> gate_;
	std::unique_ptr<SHEngine::Renderer> wall_;
	std::unique_ptr<SHEngine::Renderer> door_;

	SHEngine::GPUBuffer* gateBuffer_ = nullptr;
	SHEngine::GPUBuffer* doorBuffer_ = nullptr;

	struct VSData {
		Matrix4x4 world;
		Matrix4x4 wvp;
	};
	struct SimpleMaterial {
		Vector4 baseColor;
		int textureIndex;
	};

	Transform gateTransform_;
	Transform doorTransform_;

	float speed_ = 0.0f;
	float initSpeed_ = 0.0f;
	float maxSpeed_ = 0.0f;
	float accel_ = 0.0f;

	float topY_ = 6.2f;

	float timer_ = 0.0f;
	bool open_ = false;

	SHEngine::TextureData* first_ = nullptr;
	SHEngine::TextureData* second_ = nullptr;
};
