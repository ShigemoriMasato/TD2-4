#pragma once
#include <Scene/IScene.h>
#include <Game/Puzzle/PieceRender.h>
#include <Camera/DebugCamera.h>
#include <Tool/Grid/Grid.h>
#include <Compute/ComputeObject.h>

class ShigeScene : public IScene {
public:

	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private:

	std::unique_ptr<DebugCamera> camera_;
	std::unique_ptr<Grid> grid_;

	std::unique_ptr<SHEngine::ComputeObject> computeObject_;
	std::unique_ptr<SHEngine::RenderObject> render_;

	SkinningModelData sneekWalk_;
	Animation animation_;
	float time_ = 0.0f;
	std::vector<WellForGPU> well_;
	std::vector<VertexData> skinnedVertices_;

	std::unique_ptr<CmdObj> computeCmdObj_;
};
