#include "Field.h"
#include <numbers>

using namespace SHEngine;

void Field::Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager) {
	int modelHandle = modelManager->LoadModel("Assets/Model/field");
	auto modelData = modelManager->GetNodeModelData(modelHandle);
	auto drawData = drawDataManager->GetDrawData(modelData.drawDataIndex);

	auto& material = modelData.materials[modelData.materialIndex.front()];
	textureIndex_ = material.textureIndex;

	// 4つのモデルを初期化
	for (int i = 0; i < kFieldModelCount; ++i) {
		renders_[i] = std::make_unique<RenderObject>();
		renders_[i]->Initialize();

		// シェーダー設定
		renders_[i]->psoConfig_.vs = "Game/Field.VS.hlsl";
		renders_[i]->psoConfig_.ps = "Game/Field.PS.hlsl";
		renders_[i]->SetUseTexture(true);

		// CBVの生成
		renders_[i]->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER);
		renders_[i]->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "Color");
		renders_[i]->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");

		// 描画データを設定
		renders_[i]->SetDrawData(drawData);

		// グリッド座標を計算
		int gridX = i % kGridWidth;
		int gridY = i / kGridWidth;

		// ワールド座標を設定
		transforms_[i].position.x = gridX * kModelSpacing - (kModelSpacing * (kGridWidth - 1)) * 0.5f;
		transforms_[i].position.z = gridY * kModelSpacing - (kModelSpacing * (kGridHeight - 1)) * 0.5f;
		transforms_[i].position.y = -1.5f;

		transforms_[0].rotate = {0.0f, 0.0f, 0.0f};
		transforms_[1].rotate = {0.0f, -std::numbers::pi_v<float> / 2.0f, 0.0f};
		transforms_[2].rotate = {0.0f, std::numbers::pi_v<float> / 2.0f, 0.0f};
		transforms_[3].rotate = {0.0f, std::numbers::pi_v<float>, 0.0f};

		transforms_[i].scale = {1.0f, 1.0f, 1.0f};

		// 単位行列の代入
		wvps_[i] = Matrix4x4::Identity();
	}
}

void Field::Update(Matrix4x4 vpMatrix) {
	// 4つのモデルを更新
	for (int i = 0; i < kFieldModelCount; ++i) {
		wvps_[i] = Matrix::MakeAffineMatrix(transforms_[i].scale, transforms_[i].rotate, transforms_[i].position);
		wvps_[i] *= vpMatrix;
		renders_[i]->CopyBufferData(0, &wvps_[i], sizeof(Matrix4x4));

		Vector4 color = {1.0f, 1.0f, 1.0f, 1.0f};
		renders_[i]->CopyBufferData(1, &color, sizeof(Vector4));

		renders_[i]->CopyBufferData(2, &textureIndex_, sizeof(int));
	}
}

void Field::Draw(CmdObj* cmdObj) {
	// 4つのモデルを描画
	for (int i = 0; i < kFieldModelCount; ++i) {
		renders_[i]->Draw(cmdObj);
	}

#ifdef USE_IMGUI
	ImGui::Begin("field");
	ImGui::DragFloat("rotate0", &transforms_[0].position.y,0.01f);
	ImGui::DragFloat("rotate1", &transforms_[1].position.y,0.01f);
	ImGui::DragFloat("rotate2", &transforms_[2].position.y,0.01f);
	ImGui::DragFloat("rotate3", &transforms_[3].position.y,0.01f);
	ImGui::End();
#endif
}