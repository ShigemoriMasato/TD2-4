#pragma once
#include <Compute/ComputeObject.h>
#include <Render/Renderer.h>
#include <Camera/Camera.h>
#include <SHEngine.h>

class TitleLogo {
public:

	void Initialize(SHEngine::Engine* enigne, CmdObj* compute);
	void Update(float deltaTime, Camera* camera);
	void Draw(CmdObj* direct);

private:

	std::unique_ptr<SHEngine::ComputeObject> computeObject_;

	std::unique_ptr<SHEngine::BufferContainer> container_;
	std::unique_ptr<SHEngine::Renderer> renderer_;

	struct MatrixData {
		Matrix4x4 world;
		Matrix4x4 vp;
	} matrixData_;

	SHEngine::GPUBuffer* matrixBuffer_ = nullptr;

};
