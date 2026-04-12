#pragma once
#include <unordered_map>
#include <vector>
#include <memory>
#include <Render/RenderObject.h>
#include <Render/DrawDataManager.h>
#include <Assets/Model/ModelManager.h>
#include <Utility/Matrix.h>
#include <GameObject/Effect/Particle/Particle.h>

class ParticleDrawer final
{
public:
	struct Config
	{
		uint32_t maxModels = 128;
		uint32_t maxInstancesPerModel = 8192;
	};

public:
	void Initialize(
		SHEngine::DrawDataManager* drawDataManager,
		SHEngine::ModelManager* modelManager,
		const Config& cfg = {});

	void Clear();
	void Register(Particle* particle);
	void Draw(CmdObj* cmdObj, const Matrix4x4& vpMatrix);

private:
	struct ModelBatch
	{
		std::unique_ptr<SHEngine::RenderObject> render;
		std::vector<Particle::InstanceGpu> instances;
	};

private:
	SHEngine::DrawDataManager* drawDataManager_ = nullptr;
	SHEngine::ModelManager* modelManager_ = nullptr;

	Config config_{};

	std::vector<Particle*> particles_;
	std::unordered_map<int, ModelBatch> batches_;
};