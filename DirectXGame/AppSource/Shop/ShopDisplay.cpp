#include "ShopDisplay.h"
#include <imgui/imgui.h>
#include <numbers>

void ShopDisplay::Initialize(CmdObj* cmdObj, SHEngine::DrawData& drawData, SHEngine::TextureManager* textureManager) {
	cmdObj_ = cmdObj;
	disp_ = std::make_unique<SHEngine::Screen::MultiDisplay>();
	disp_->Initialize(624, 352, 0xff0000ff, textureManager);
	
	render_ = std::make_unique<SHEngine::RenderObject>("ShopDisplay");
	render_->Initialize();
	render_->SetDrawData(drawData);
	render_->psoConfig_.vs = "Simple.VS.hlsl";
	render_->psoConfig_.ps = "PostEffect/Simple.PS.hlsl";
	render_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "WVP");
	render_->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");
	render_->SetUseTexture(true);

	render_->psoConfig_.depthStencilID = SHEngine::PSO::DepthStencilID::Transparent;

	transform_.scale = { 624.0f / 360.0f, 352.0f / 640.0f, 0.0f };
	transform_.rotate.z = std::numbers::pi_v<float> / 2.0f;
	transform_.position.z = 0.5f;
	transform_.position.x = 224.0f / 640.0f - 1.0f;
}

void ShopDisplay::Update() {
	Matrix4x4 wvp = Matrix::MakeScaleMatrix(transform_.scale) * Matrix::MakeRotationMatrix(transform_.rotate) * Matrix::MakeTranslationMatrix(transform_.position);
	int textureIndex = disp_->GetTextureData()->GetOffset();

	render_->CopyBufferData(0, &wvp, sizeof(wvp));
	render_->CopyBufferData(1, &textureIndex, sizeof(textureIndex));
}

void ShopDisplay::PreDraw() {
	disp_->PreDraw(cmdObj_);
}

void ShopDisplay::PostDraw() {
	disp_->PostDraw(cmdObj_);
}

void ShopDisplay::Draw() {
	render_->Draw(cmdObj_);

#ifdef USE_IMGUI

	ImGui::Begin("ShopDisplay");

	ImGui::DragFloat3("Scale", &transform_.scale.x, 0.01f);
	ImGui::DragFloat3("Rotate", &transform_.rotate.x, 0.01f);
	ImGui::DragFloat3("Position", &transform_.position.x, 0.01f);

	ImGui::End();

#endif
}
