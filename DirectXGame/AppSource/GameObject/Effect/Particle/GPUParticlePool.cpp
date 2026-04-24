#include "GPUParticlePool.h"


void GPUParticlePool::Initialize(SHEngine::DrawData& plane, CmdObj* cmdObj, uint32_t maxParticles)
{
	container_ = std::make_unique<SHEngine::BufferContainer>();
	particle_ = std::make_unique<SHEngine::Renderer>(plane);
	initialize_ = std::make_unique<SHEngine::ComputeObject>();

	pool_.freeList = container_->Create(BufferType::UAV, sizeof(uint32_t) * maxParticles);
	pool_.freeListIndex = container_->Create(BufferType::UAV, sizeof(uint32_t));
	pool_.position = container_->Create(BufferType::SRV_UAV, sizeof(Vector4) * maxParticles);
	pool_.color = container_->Create(BufferType::SRV_UAV, sizeof(Vector4) * maxParticles);
	pool_.type = container_->Create(BufferType::SRV_UAV, sizeof(uint32_t) * maxParticles);
	pool_.particleNum = container_->Create(BufferType::CBV, sizeof(uint32_t));
	pool_.deltaTime = container_->Create(BufferType::CBV, sizeof(float) * maxParticles);
	pool_.maxParticles = maxParticles;

	cameraBuffer_ = container_->Create(BufferType::CBV, sizeof(CameraData));

	pool_.particleNum->CopyBuffer(&maxParticles, sizeof(uint32_t));

	initialize_->SetShader("Particle/Pool/Init.CS.hlsl");
	initialize_->SetGPUBuffers(BufferType::UAV, { pool_.freeList, pool_.freeListIndex, pool_.type });
	initialize_->SetThreadGroupSize(maxParticles / 128 + 1);
	initialize_->Execute(cmdObj);

	particle_->SetVS("Particle/Pool.VS.hlsl");
	particle_->SetPS("Particle/Pool.PS.hlsl");
	particle_->SetGPUBuffer(pool_.position, ShaderType::VERTEX_SHADER, BufferType::SRV);
	particle_->SetGPUBuffer(pool_.type, ShaderType::VERTEX_SHADER, BufferType::SRV);
	particle_->SetGPUBuffer(cameraBuffer_, ShaderType::VERTEX_SHADER, BufferType::CBV);
	particle_->SetGPUBuffer(pool_.color, ShaderType::PIXEL_SHADER, BufferType::SRV);
}

void GPUParticlePool::Update(Camera* camera, float deltaTime)
{
	camera_.vp = camera->GetVPMatrix();
	camera_.billboard = camera->GetBillboardMatrix();
	cameraBuffer_->CopyBuffer(&camera_, sizeof(CameraData));
}

void GPUParticlePool::Draw(CmdObj* cmdObj)
{
	particle_->Draw(cmdObj);
}
