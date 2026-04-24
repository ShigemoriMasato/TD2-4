#pragma once
#include <Render/Renderer.h>
#include <Compute/ComputeObject.h>
#include <Camera/Camera.h>

struct Pool
{
	SHEngine::GPUBuffer* freeList = nullptr;
	SHEngine::GPUBuffer* freeListIndex = nullptr;
	SHEngine::GPUBuffer* position = nullptr;
	SHEngine::GPUBuffer* color = nullptr;
	SHEngine::GPUBuffer* type = nullptr;
	SHEngine::GPUBuffer* particleNum = nullptr;
	SHEngine::GPUBuffer* deltaTime = nullptr;
	uint32_t maxParticles = 0;
};



class GPUParticlePool
{
public:
	void Initialize(SHEngine::DrawData& plane, CmdObj* cmdObj, uint32_t maxParticles);
	void Update(Camera* camera, float deltaTime);
	void Draw(CmdObj* cmdObj);

private:
	std::unique_ptr<SHEngine::Renderer> particle_;
	std::unique_ptr<SHEngine::BufferContainer> container_;
	std::unique_ptr<SHEngine::ComputeObject> initialize_;

	struct CameraData
	{
		Matrix4x4 vp;
		Matrix4x4 billboard;
	}camera_;

	SHEngine::GPUBuffer* cameraBuffer_ = nullptr;

	Pool pool_;
}
