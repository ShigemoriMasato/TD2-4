#include "PieceRender.h"

using namespace SHEngine;

void PieceRender::Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager) {

	renderObjects_.resize(maxPieceKindNum_);

	//~~~~~~~~~~~~~~~~~~~~~~~~~
	//Pieceに必要なもの
	//~~~~~~~~~~~~~~~~~~~~~~~~~
	// モデルの描画
	// >>>テクスチャの仕様
	// >>>各モデルごとに描画を分ける
	// >>>色の指定
	// 
	// アニメーションは行列でできる
	// 
	// 送るもの
	// worldマトリックス
	// テクスチャ
	// テクスチャインデックス
	// 光系のなんか(とりあえずDirectionalLightひとつ)
	//=========================

	for (int i = 0; i < maxPieceKindNum_; i++) {
		renderObjects_[i] = std::make_unique<RenderObject>("PieceRenderObject" + std::to_string(i));
		renderObjects_[i]->Initialize();
		renderObjects_[i]->SetUseTexture(true);

		renderObjects_[i]->psoConfig_.vs = "Game/Piece.VS.hlsl";
		renderObjects_[i]->psoConfig_.ps = "Game/Piece.PS.hlsl";

		renderObjects_[i]->CreateSRV(sizeof(Matrix4x4), maxInstanceNum_, ShaderType::VERTEX_SHADER, "Matrix");
		renderObjects_[i]->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "ViewProj");
		renderObjects_[i]->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");
		renderObjects_[i]->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "Color");
		renderObjects_[i]->CreateCBV(sizeof(DirectionalLight), ShaderType::PIXEL_SHADER, "DirectionalLight");
	}
}

void PieceRender::SetPiece(const std::vector<Piece>& piece) {
	
	for (const auto& p : piece) {
		int modelID = p.GetModelID();

		const auto& it = modelRenderMap_.find(modelID);
		if (it == modelRenderMap_.end()) {
			RegisterRenderObject(modelID);
		}


	}
}

void PieceRender::RegisterRenderObject(int modelID) {
	if (registerNum_ >= maxPieceKindNum_) {
		getLogger("Error")->error("PieceRender::RegisterRenderObject: Item描画の種類数が超過");

		//新しくRenderObjectを作る
		auto& renderObj = renderObjects_.emplace_back();
		renderObj = std::make_unique<RenderObject>("TmpObject");
		renderObj->Initialize();
		renderObj->SetUseTexture(true);

		renderObj->psoConfig_.vs = "Game/Piece.VS.hlsl";
		renderObj->psoConfig_.ps = "Game/Piece.PS.hlsl";

		renderObj->CreateSRV(sizeof(Matrix4x4), maxInstanceNum_, ShaderType::VERTEX_SHADER, "Matrix");
		renderObj->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "ViewProj");
		renderObj->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");
		renderObj->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "Color");
		renderObj->CreateCBV(sizeof(DirectionalLight), ShaderType::PIXEL_SHADER, "DirectionalLight");
	}

	auto render = renderObjects_[registerNum_++].get();

	//頂点情報の設定
	auto modelData = modelManager_->GetNodeModelData(modelID);
	auto drawData = drawDataManager_->GetDrawData(modelData.drawDataIndex);
	render->SetDrawData(drawData);

	modelRenderMap_[modelID] = render;
	matrixMap_[modelID] = std::vector<Matrix>(maxInstanceNum_, Matrix());
}
