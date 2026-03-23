#include "ShopDisplay.h"
#include <numbers>

void ShopDisplay::Initialize(CmdObj* cmdObj, SHEngine::DrawData& drawData, SHEngine::TextureManager* textureManager) {
	cmdObj_ = cmdObj;
	disp_ = std::make_unique<SHEngine::Screen::MultiDisplay>();
	disp_->Initialize(640, 720, 0x000000ff, textureManager);
	
	render_ = std::make_unique<SHEngine::RenderObject>("ShopDisplay");
	render_->Initialize();
	render_->SetDrawData(drawData);
	render_->psoConfig_.vs = "Simple.VS.hlsl";
	render_->psoConfig_.ps = "PostEffect/Simple.PS.hlsl";
	render_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "WVP");
	render_->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");
	render_->SetUseTexture(true);

	render_->psoConfig_.depthStencilID = SHEngine::PSO::DepthStencilID::Transparent;
}

void ShopDisplay::Update() {
	Matrix4x4 wvp = Matrix::MakeScaleMatrix({2.0f, 1.0f, 2.0f}) * Matrix::MakeRotationZMatrix(std::numbers::pi_v<float> / 2.0f) * Matrix::MakeTranslationMatrix({-0.5f, 0, 1.0f});
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
}
