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


	modelManager_ = modelManager;
	drawDataManager_ = drawDataManager;

}

void PieceRender::SetPiece(const std::vector<Piece>& piece) {
	for(const auto& [modelID, render] : modelRenderMap_) {
		render->instanceNum_ = 0;
		matrixMap_[modelID].clear();
	}

	for (const auto& p : piece) {
		int modelID = p.GetModelID();

		const auto& it = modelRenderMap_.find(modelID);
		if (it == modelRenderMap_.end()) {
			RegisterRenderObject(modelID);
		}

		matrixMap_[modelID].push_back(Matrix::MakeTranslationMatrix(p.GetPosition()));
		modelRenderMap_[modelID]->instanceNum_++;
	}
}

void PieceRender::Update(const Matrix4x4& viewProj) {
	for (const auto& [modelID, render] : modelRenderMap_) {
		const auto& matrices = matrixMap_[modelID];
		if (matrices.empty()) continue;
		render->CopyBufferData(0, matrices.data(), sizeof(Matrix4x4) * matrices.size());
		render->CopyBufferData(1, &viewProj, sizeof(Matrix4x4));

		auto model = modelManager_->GetNodeModelData(modelID);
		auto material = model.materials[model.materialIndex.front()];
		int textureIndex = material.textureIndex;
		Vector4 color = material.color;

		render->CopyBufferData(2, &textureIndex, sizeof(int));
		render->CopyBufferData(3, &color, sizeof(Vector4));

		render->CopyBufferData(4, &directionalLight_, sizeof(DirectionalLight));
	}
}

void PieceRender::Draw(CmdObj* cmdObj) {
	for (const auto& [modelID, render] : modelRenderMap_) {
		render->Draw(cmdObj);
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
	matrixMap_[modelID].reserve(maxInstanceNum_);
}
