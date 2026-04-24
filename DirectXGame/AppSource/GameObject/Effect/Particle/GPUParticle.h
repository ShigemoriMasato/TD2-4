#pragma once
#include <Compute/ComputeObject.h>
#include <SHEngine.h>
#include <GameObject/Effect/Particle/GPUParticlePool.h>


class GPUParticle
{
public:
	void Initialize(SHEngine::Engine* engine);
	void Update(float dt);
	void Draw(const Matrix4x4& vp);

private:
	SHEngine::Engine* engine_;

	std::vector<std::unique_ptr<CmdObj>> compute_;
	std::unique_ptr<CmdObj> direct_;

	std::unique_ptr<GPUParticlePool> pool_;
};

