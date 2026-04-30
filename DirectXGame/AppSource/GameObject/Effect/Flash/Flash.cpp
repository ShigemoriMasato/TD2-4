#include "Flash.h"

using namespace SHEngine;

void Flash::Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager) {
	int modelHandle = modelManager->LoadModel("Assets/.EngineResource/Model/plane");
	auto modelData = modelManager->GetNodeModelData(modelHandle);
	DrawData data = drawDataManager->GetDrawData(modelData.drawDataIndex);
	
	render_ = std::make_unique<RenderObject>("FlashEffect");
	render_->Initialize();
	render_->psoConfig_.vs = "Simple.VS.hlsl";
	render_->psoConfig_.ps = "Color.PS.hlsl";
	render_->SetDrawData(data);
	render_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "WVP");
	render_->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "Color");
	transform_.scale = {1280.0f, 720.0f, 0.0f};
	transform_.rotate = {0, 0, 0};
	transform_.position = {0.0f, 0.0f, 0};
}

void Flash::Update(Matrix4x4 vpMatrix, float deltaTime) {
	// フラッシュ減衰
	if (flashIntensity_ > 0.0f) {
		flashIntensity_ -= flashDecaySpeed_ * deltaTime;
		flashIntensity_ = std::max(flashIntensity_, 0.0f);
	}

	Matrix4x4 wvp = Matrix::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.position);
	Vector4 color = {1.0f, 1.0f, 1.0f, flashIntensity_};
	int textureIndex = 0;

	render_->CopyBufferData(0, &wvp, sizeof(Matrix4x4));
	render_->CopyBufferData(1, &color, sizeof(Vector4));
}

void Flash::Draw(CmdObj* cmdObj) { render_->Draw(cmdObj); }

void Flash::Trigger() {
	if (isActive_)
		return;

	flashIntensity_ = 1.0f;
	isActive_ = true;
}
