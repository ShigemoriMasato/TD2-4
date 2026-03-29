#pragma once
#include <Scene/iscene.h>
#include <Render/Font/Text.h>
#include <Camera/DebugCamera.h>
#include <Render/Renderer.h>
#include <Compute/ComputeObject.h>

class TechnicalTestScene : public IScene {
public:

	virtual void Initialize() override;
	virtual std::unique_ptr<IScene> Update() override;
	virtual void Draw() override;

private:

	std::unique_ptr<DebugCamera> debugCamera_ = nullptr;

	std::unique_ptr<SHEngine::Text> text_ = nullptr;
	Transform textTransform_ = {};

	std::unique_ptr<CmdObj> computeCmdObj_ = nullptr;


	std::unique_ptr<SHEngine::Renderer> renderer_ = nullptr;
	std::unique_ptr<SHEngine::ComputeObject> skinningCompute_ = nullptr;
	std::vector<std::unique_ptr<SHEngine::GPUBuffer>> gpuBuffers_ = {};

	struct SkinningTransformMatrix {
		Matrix4x4 world;
		Matrix4x4 vp;
	};

	struct SkinningInformation {
		uint32_t numVertices;
	};

	SkinningModelData* skinningModelData_ = nullptr;
	Animation animation_ = {};
	float animationTime_ = 0.0f;
	std::vector<WellForGPU> skinningPalette_ = {};
	SkinningTransformMatrix skinningTransform_ = {};
	SkinningInformation skinningInfo_ = {};

	static constexpr int kBufferTransform = 0;
	static constexpr int kBufferTextureIndex = 1;
	static constexpr int kBufferMatrixPalette = 2;
	static constexpr int kBufferCSInfo = 3;
	static constexpr int kBufferCSVertices = 4;
	static constexpr int kBufferCSInfluences = 5;
	static constexpr int kBufferCSOutput = 6;
};
