#include "ParameterRender.h"
#include <numbers>

using namespace SHEngine;

void ParameterRender::Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, SHEngine::Engine* engine) {
	int modelHandle = modelManager->LoadModel("Assets/.EngineResource/Model/plane");
	auto modelData = modelManager->GetNodeModelData(modelHandle);
	auto drawData = drawDataManager->GetDrawData(modelData.drawDataIndex);

	// パラメータのモデルを初期化
	for (int i = 0; i < kParameterCount; ++i) {
		renders_[i] = std::make_unique<RenderObject>();
		renders_[i]->Initialize();

		// シェーダー設定
		renders_[i]->psoConfig_.vs = "Game/Field.VS.hlsl";
		renders_[i]->psoConfig_.ps = "Game/Field.PS.hlsl";
		renders_[i]->psoConfig_.isSwapChain = false;
		renders_[i]->SetUseTexture(true);

		// テクスチャの読み込み
		auto textureManager = engine->GetTextureManager();
		textureIndexes_[i] = textureManager->LoadTexture("PlayerParameter/" + texturePaths[i]);

		// CBVの生成
		renders_[i]->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER);
		renders_[i]->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "Color");
		renders_[i]->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");

		// 描画データを設定
		renders_[i]->SetDrawData(drawData);

		// ワールド座標を設定
		transforms_[i].position.x = 0.0f;
		transforms_[i].position.y = -2.0f * i + startPosY_;
		transforms_[i].position.z = 0.0f;

		// 回転
		transforms_[i].rotate = {0.0f, 0.0f, std::numbers::pi_v<float> / 2.0f};

		// スケール
		transforms_[i].scale = {2.0f, 2.0f, 1.0f};

		// 単位行列の代入
		wvps_[i] = Matrix4x4::Identity();
	}
}

void ParameterRender::Update(Matrix4x4 vpMatrix) {
	for (int i = 0; i < kParameterCount; ++i) {
	    transforms_[i].position.y = -2.0f * i + startPosY_;
	    transforms_[i].position.z = posZ_;
	}
	for (int i = 0; i < kParameterCount; ++i) {
		wvps_[i] = Matrix::MakeAffineMatrix(transforms_[i].scale, transforms_[i].rotate, transforms_[i].position);
		wvps_[i] *= vpMatrix;
		renders_[i]->CopyBufferData(0, &wvps_[i], sizeof(Matrix4x4));

		Vector4 color = {1.0f, 1.0f, 1.0f, 1.0f};
		renders_[i]->CopyBufferData(1, &color, sizeof(Vector4));

		renders_[i]->CopyBufferData(2, &textureIndexes_[i], sizeof(int));
	}
}

void ParameterRender::Draw(CmdObj* cmdObj) {
	for (int i = 0; i < kParameterCount; ++i) {
		renders_[i]->Draw(cmdObj);
	}

#ifdef USE_IMGUI
	ImGui::Begin("ParameterRender");
	ImGui::DragFloat("StartPosY", &startPosY_, 0.01f);
	ImGui::DragFloat("PosZ", &posZ_, 0.01f);
	for (int i = 0; i < kParameterCount; ++i) {
		std::string scaleLabel = "Scale" + std::to_string(i);
		std::string rotateLabel = "Rotate" + std::to_string(i);
		std::string positionLabel = "Position" + std::to_string(i);
		ImGui::DragFloat3(scaleLabel.c_str(), &transforms_[i].scale.x, 0.01f);
		ImGui::DragFloat3(rotateLabel.c_str(), &transforms_[i].rotate.x, 0.01f);
		ImGui::DragFloat3(positionLabel.c_str(), &transforms_[i].position.x, 0.01f);
	}
	ImGui::End();
#endif
}