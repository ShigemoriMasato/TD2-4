#include "ShopDisplay.h"
#include <imgui/imgui.h>
#include <numbers>

void ShopDisplay::Initialize(CmdObj* cmdObj, SHEngine::DrawData& drawData, SHEngine::TextureManager* textureManager) {
	cmdObj_ = cmdObj;
	disp_ = std::make_unique<SHEngine::Screen::MultiDisplay>();
	disp_->Initialize(624, 352, 0x000000ff, textureManager);
	
	render_ = std::make_unique<SHEngine::RenderObject>("ShopDisplay");
	render_->Initialize();
	render_->SetDrawData(drawData);
	render_->psoConfig_.vs = "Simple.VS.hlsl";
	render_->psoConfig_.ps = "PostEffect/Simple.PS.hlsl";
	render_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "WVP");
	render_->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");
	render_->SetUseTexture(true);

	render_->psoConfig_.depthStencilID = SHEngine::PSO::DepthStencilID::Transparent;

	SetTransform({ 48.0f, 48.0f }, { 352.0f, 624.0f });
}

void ShopDisplay::SetTransform(const Vector2& leftTop, const Vector2& size) {
	transform_.scale = { size.y / 360.0f, size.x / 640.0f, 0.0f };
	transform_.rotate.z = std::numbers::pi_v<float> / 2.0f;

	Vector2 center = leftTop + size * 0.5f;

	transform_.position.z = 0.5f;
	transform_.position.x = center.x / 640.0f - 1.0f;
	transform_.position.y = center.y / -360.0f + 1.0f;
}

void ShopDisplay::Update() {
	Matrix4x4 wvp = Matrix::MakeScaleMatrix(transform_.scale) * Matrix::MakeRotationMatrix(transform_.rotate) * Matrix::MakeTranslationMatrix(transform_.position);
	int textureIndex = disp_->GetTextureData()->GetOffset();

	render_->CopyBufferData(0, &wvp, sizeof(wvp));
	render_->CopyBufferData(1, &textureIndex, sizeof(textureIndex));
}

void ShopDisplay::PreDraw() {
	cmdObj_->SetRenderTarget(disp_.get(), true);
}

void ShopDisplay::ToPresent() {
	disp_->ToPresent(cmdObj_);
}

void ShopDisplay::Draw() {
	render_->Draw(cmdObj_);

#ifdef USE_IMGUI

#endif
}
