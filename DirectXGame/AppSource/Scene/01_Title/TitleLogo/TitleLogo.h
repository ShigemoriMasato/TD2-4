#pragma once
#include <Compute/ComputeObject.h>
#include <Render/Renderer.h>
#include <Camera/Camera.h>
#include <SHEngine.h>
#include <random>

struct Wave {
	Vector3 position;
	float speed;

	Vector3 color;
	float intensity;

	float lifetime = 10000000.0f;
	float decvayRate;
	float maxlifetime;
	float thickness;
};

class TitleLogo {
public:

	~TitleLogo();

	void Initialize(SHEngine::Engine* enigne, CmdObj* compute);
	void Update(float deltaTime, Camera* camera, CmdObj* compute);
	void Draw(CmdObj* direct);

	void AddWave(const Wave& wave);

	void Mist();
	void Default();

private:

	void Save();
	void Load();

	bool isSave_ = true;

	std::unique_ptr<SHEngine::ComputeObject> init_;
	std::unique_ptr<SHEngine::ComputeObject> emit_;
	std::unique_ptr<SHEngine::ComputeObject> update_;

	struct EmitData {
		Vector3 fieldSize;
		float speed;
		float lifeTime;
		int emitNum;
		uint32_t seed;
		int textureIndex;
	} emitData_;
	EmitData defaultData_;
	EmitData mistData_;
	SHEngine::GPUBuffer* emitBuffer_ = nullptr;

	struct UpdateData {
		float deltaTime;
		Vector3 color;
		float lifetime;
		Vector3 fieldSize;
		Matrix4x4 worldMatrix;
	}updateData_;
	Transform parentTransform_;
	SHEngine::GPUBuffer* updateBuffer_ = nullptr;



	std::unique_ptr<SHEngine::BufferContainer> container_;
	std::unique_ptr<SHEngine::Renderer> renderer_;

	SHEngine::GPUBuffer* sizeBuffer_ = nullptr;
	float size_ = 0.01f;

	struct MatrixData {
		Matrix4x4 vpMatrix;
		Matrix4x4 billboardMatrix;
	}matrixData_;
	SHEngine::GPUBuffer* gateBuffer_ = nullptr;

	
	
	std::mt19937 randomEngine_;
	std::uniform_int_distribution<uint32_t> randomDist_{0, UINT32_MAX};

	std::vector<Wave> waves_;
	SHEngine::GPUBuffer* waveBuffer_ = nullptr;
};
