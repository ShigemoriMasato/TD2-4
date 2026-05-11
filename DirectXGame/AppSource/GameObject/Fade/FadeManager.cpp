#include "FadeManager.h"

using namespace SHEngine;

void FadeManager::Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager) {
	int modelHandle = modelManager->LoadModel("Assets/.EngineResource/Model/plane");
	auto modelData = modelManager->GetNodeModelData(modelHandle);
	DrawData data = drawDataManager->GetDrawData(modelData.drawDataIndex);

	render_ = std::make_unique<RenderObject>("FadeSprite");
	render_->Initialize();
	render_->psoConfig_.vs = "Simple.VS.hlsl";
	render_->psoConfig_.ps = "Color.PS.hlsl";
	render_->SetDrawData(data);
	render_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "WVP");
	render_->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "Color");
	transform_.scale = {1280.0f, 720.0f, 0.0f};
	transform_.rotate = {0, 0, 0};
	transform_.position = {0.0f, 0.0f, 0};

	isFinished_ = false;
}

void FadeManager::Update(Matrix4x4 vpMatrix, float deltaTime) {
	if (isFinished_)
		return;

	UpdateFade(deltaTime);

	Matrix4x4 wvp = Matrix::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.position);
	Vector4 color = {0.0f, 0.0f, 0.0f, alpha_};

	render_->CopyBufferData(0, &wvp, sizeof(Matrix4x4));
	render_->CopyBufferData(1, &color, sizeof(Vector4));
}

void FadeManager::Draw(CmdObj* cmdObj) { render_->Draw(cmdObj); }

void FadeManager::StartFadeOut(bool notifyFinish) {
	if (state_ != FadeState::Idle)
		return;

	state_ = FadeState::FadeOut;
	notifyFinish_ = notifyFinish;
	isFinished_ = false;
	alpha_ = 1.0f;
}

void FadeManager::StartFadeIn(bool notifyFinish) {
	if (state_ != FadeState::Idle)
		return;

	state_ = FadeState::FadeIn;
	notifyFinish_ = notifyFinish;
	isFinished_ = false;
	alpha_ = 0.0f;
}

void FadeManager::UpdateFade(float deltaTime) {
	switch (state_) {
	case FadeState::FadeIn:
		alpha_ += fadeSpeed_ * deltaTime;
		if (alpha_ >= 1.0f) {
			alpha_ = 1.0f;
			state_ = FadeState::Idle;
			if (notifyFinish_) {
				isFinished_ = true;
			}
		}
		break;

	case FadeState::FadeOut:
		alpha_ -= fadeSpeed_ * deltaTime;
		if (alpha_ <= 0.0f) {
			alpha_ = 0.0f;
			state_ = FadeState::Idle;
			if (notifyFinish_) {
				isFinished_ = true;
			}
		}
		break;

	case FadeState::Idle:
	default:
		break;
	}
}
