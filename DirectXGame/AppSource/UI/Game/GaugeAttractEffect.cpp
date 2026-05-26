#include "GaugeAttractEffect.h"
#include <algorithm>

#ifdef USE_IMGUI
#include <imgui/imgui.h>
#endif

using namespace SHEngine;

void GaugeAttractEffect::Initialize(SHEngine::DrawDataManager* drawDataManager, SHEngine::ModelManager* modelManager, SHEngine::TextureManager* textureManager, Vector3 startPos) {
	transform_.scale = {108.0f, 64.0f, 1.0f};
	transform_.rotate = {0.0f, 0.0f, 0.0f};
	transform_.position = startPos;
	explosionScale_ = transform_.scale;

	int modelHandle = modelManager->LoadModel("Assets/.EngineResource/Model/plane");
	auto modelData = modelManager->GetNodeModelData(modelHandle);
	DrawData data = drawDataManager->GetDrawData(modelData.drawDataIndex);

	explosionRender_ = std::make_unique<RenderObject>("ExplosionEffect");
	explosionRender_->Initialize();
	explosionRender_->psoConfig_.vs = "Simples.VS.hlsl";
	explosionRender_->psoConfig_.ps = "Game/AfterImage.PS.hlsl";
	explosionRender_->SetDrawData(data);
	explosionRender_->CreateSRV(sizeof(Matrix4x4), 2, ShaderType::VERTEX_SHADER, "WVP");
	explosionRender_->CreateSRV(sizeof(Vector4), 2, ShaderType::PIXEL_SHADER, "Color");
	explosionRender_->CreateSRV(sizeof(int), 2, ShaderType::PIXEL_SHADER, "TextureIndex");
	explosionRender_->SetUseTexture(true);
	explosionRender_->instanceNum_ = 2;
	ringTexture_ = textureManager->LoadTexture("Ring.png");
	crossTexture_ = textureManager->LoadTexture("Cross.png");

	scaleAnim_.Start(transform_.scale, transform_.scale * 8.0f, 1.0f, EaseType::EaseOutCubic);
	alphaAnim_.Start(1.0, 0.0f, 1.0f, EaseType::EaseOutCubic);
}

void GaugeAttractEffect::Update(Matrix4x4 vpMatrix, float deltaTime) {
	if (isFinished_)
		return;

	std::vector<Matrix4x4> wvpMatrices(2);
	std::vector<Vector4> colors(2);
	std::vector<int> textures(2);

	textures[0] = ringTexture_;
	textures[1] = crossTexture_;

	bool scaleActive = scaleAnim_.Update(deltaTime, explosionScale_);
	bool opacityActive = alphaAnim_.Update(deltaTime, alpha_);

	// アニメーションが両方終わったら終了フラグを立てる
	if (!scaleActive && !opacityActive) {
		isFinished_ = true;
	}

	for (int i = 0; i < 2; ++i) {
		// 行列と色の更新
		Matrix4x4 world = Matrix::MakeAffineMatrix(explosionScale_, {0, 0, 0}, transform_.position);
		Matrix4x4 wvp = world * vpMatrix;
		Vector4 color = {1.0f, 0.0f, 0.0f, alpha_};

		wvpMatrices[i] = world * vpMatrix;
		colors[i] = color;

		explosionRender_->CopyBufferData(0, wvpMatrices.data(), sizeof(Matrix4x4) * 2);
		explosionRender_->CopyBufferData(1, colors.data(), sizeof(Vector4) * 2);
		explosionRender_->CopyBufferData(2, textures.data(), sizeof(int) * 2);
	}
}

void GaugeAttractEffect::Draw(CmdObj* cmdObj) {
	if (isFinished_)
		return;

	explosionRender_->Draw(cmdObj);
}