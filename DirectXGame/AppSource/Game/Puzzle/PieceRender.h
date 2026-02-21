#pragma once
#include <Game/Puzzle/Piece.h>
#include <Render/RenderObject.h>
#include <Assets/Model/ModelManager.h>
#include <Utility/DataStructures.h>

class PieceRender {
public:
	
	void Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager);

	void SetPiece(const std::vector<Piece>& piece);
	void Update(const Matrix4x4& viewProj);
	
	void Draw(const CmdObj* cmdObj);

private:

	void RegisterRenderObject(int modelID);

	SHEngine::ModelManager* modelManager_;
	SHEngine::DrawDataManager* drawDataManager_;

private:

	//RenderObject関係

	std::vector<std::unique_ptr<SHEngine::RenderObject>> renderObjects_;
	//モデルIndex->RenderObjectのマップ
	std::map<int, SHEngine::RenderObject*> modelRenderMap_;
	const int maxPieceKindNum_ = 128;
	const int maxInstanceNum_ = 32;
	int registerNum_ = 0;

	//Shaderへのデータ関係
	struct Matrix {
		Matrix4x4 world = Matrix4x4::Identity();
	};
	std::map<int, std::vector<Matrix>> matrixMap_;	//RenderObjectIndex->Matrixのマップ

};
