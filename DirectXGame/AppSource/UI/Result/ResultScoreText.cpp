#include "ResultScoreText.h"
#include <imgui/imgui.h>

using namespace SHEngine;

void ResultScoreText::Initialize(DrawData& drawData, int killCount, KeyManager* keyManager) {
	text_ = std::make_unique<Text>(64);
	text_->Initialize(drawData, "YDWbananaslipplus.otf", 64);
	text_->SetColor({1.0f, 1.0f, 1.0f, 1.0f});
	text_->SetSize(1.5f);
	std::wstring killCountWStr = std::format(L"Kill: {:d}", killCount);
	text_->SetText(killCountWStr);
	transform_.position = {360.0f, -360.0f, 0.0f};

	render_ = std::make_unique<RenderObject>("ResultTimer");
	render_->Initialize();
	render_->psoConfig_.vs = "Simple.VS.hlsl";
	render_->psoConfig_.ps = "Color.PS.hlsl";
	render_->SetDrawData(drawData);
	render_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "WVP");
	render_->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "Color");

	spriteTransform_.scale = {0.0f, 80.0f, 0.0f};
	spriteTransform_.position = {spritePosX_, transform_.position.y, 0.0f};
	spritePosX_ = transform_.position.x - spriteWidth_;

	keyManager_ = keyManager;
}

void ResultScoreText::Update(Matrix4x4 vpMatrix, float deltaTime) {
#ifdef USE_IMGUI
	ImGui::Begin("ScoreText");
	ImGui::DragFloat2("Kill Count Text Position", &transform_.position.x, 1.0f);
	ImGui::DragFloat3("SpriteScale", &spriteTransform_.scale.x, 1.0f);
	ImGui::End();
#endif

	bool wasPlayingBefore = scaleBeforeAnim_.anim.GetIsActive();
	bool playingBefore = scaleBeforeAnim_.anim.Update(deltaTime, scaleBeforeAnim_.temp);
	bool playingAfter = scaleAfterAnim_.anim.Update(deltaTime, scaleAfterAnim_.temp);

	if (wasPlayingBefore && !playingBefore) {
		scaleAfterAnim_.anim.Start(spriteTransform_.scale.x, 0.0f, 0.2f, EaseType::EaseOutCubic);
		isTextRender_ = true;
	}

	if (playingBefore) {
		spriteTransform_.scale.x = scaleBeforeAnim_.temp;
		float scale = spriteTransform_.scale.x;
		float leftFixed = spritePosX_;
		spriteTransform_.position.x = leftFixed + (spriteWidth_ + scale * 0.5f);
	} else if (playingAfter) {
		spriteTransform_.scale.x = scaleAfterAnim_.temp;
		float scale = spriteTransform_.scale.x;
		float rightFixed = spritePosX_ + spriteWidth_;
		spriteTransform_.position.x = rightFixed + (spriteWidth_ - scale * 0.5f);
	}

	spriteTransform_.position.y = transform_.position.y;
	spriteTransform_.rotate = transform_.rotate;
	spritePosX_ = transform_.position.x - spriteWidth_;

	Matrix4x4 wvp = Matrix::MakeAffineMatrix(spriteTransform_.scale, spriteTransform_.rotate, spriteTransform_.position);
	wvp *= vpMatrix;
	Vector4 color = {1.0f, 1.0f, 1.0f, 1.0f};
	render_->CopyBufferData(0, &wvp, sizeof(Matrix4x4));
	render_->CopyBufferData(1, &color, sizeof(Vector4));

	text_->SetTransform(transform_);
	text_->Update(vpMatrix);
}

void ResultScoreText::Draw(CmdObj* cmdObj) {
	if (isTextRender_) {
		text_->Draw(cmdObj);
	}
	render_->Draw(cmdObj);
}

void ResultScoreText::StartAnimation() {
	scaleBeforeAnim_.anim.Start(0.0f, 300.0f, 0.2f, EaseType::EaseOutCubic);
	isTextRender_ = false;
}
