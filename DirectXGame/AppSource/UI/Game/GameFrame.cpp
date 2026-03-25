#include "GameFrame.h"
#include <numbers>
#include <imgui/imgui.h>

void GameFrame::Initialize(SHEngine::DrawData& drawData, int textureIndex) {
	render_ = std::make_unique<SHEngine::RenderObject>("GameFrame");
	render_->Initialize();
	render_->SetDrawData(drawData);
	render_->psoConfig_.vs = "Simple.VS.hlsl";
	render_->psoConfig_.ps = "PostEffect/Simple.PS.hlsl";
	render_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "WVP");
	render_->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");
	render_->SetUseTexture(true);
	transform_.scale = { 1.0f, 1.0f, 0.0f };
	render_->psoConfig_.depthStencilID = SHEngine::PSO::DepthStencilID::Transparent;

	transform_.scale = { 2.0f, 2.0f, 0.0f };
	transform_.rotate.z = std::numbers::pi_v<float> / 2.0f;
	transform_.position.z = 0.5f;
	textureIndex_ = textureIndex;
}

void GameFrame::Update() {
	Matrix4x4 wvp = Matrix::MakeScaleMatrix(transform_.scale) * Matrix::MakeRotationMatrix(transform_.rotate) * Matrix::MakeTranslationMatrix(transform_.position);
	render_->CopyBufferData(0, &wvp, sizeof(wvp));
	render_->CopyBufferData(1, &textureIndex_, sizeof(textureIndex_));
}

void GameFrame::Draw(CmdObj* cmdObj) {
	render_->Draw(cmdObj);
}
