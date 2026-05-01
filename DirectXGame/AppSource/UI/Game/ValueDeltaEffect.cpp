#include "ValueDeltaEffect.h"

using namespace SHEngine;

void ValueDeltaEffect::Initialize(SHEngine::DrawData& drawData, const std::string& fontPath, int fontSize) {
	text_ = std::make_unique<Text>();
	text_->Initialize(drawData, fontPath, fontSize, "RerollNotificationText");
	text_->SetSize(4.0f);
}

void ValueDeltaEffect::Trigger(bool isIncrease, const Vector3& startPos) {
	isActive_ = true;

	if (isIncrease) {
		text_->SetText(L"+1");
		baseColor_ = {0.2f, 1.0f, 0.2f, 1.0f};
	} else {
		text_->SetText(L"-1");
		baseColor_ = {1.0f, 0.2f, 0.2f, 1.0f};
	}

	currentPos_ = startPos;
	currentAlpha_ = 0.5f;

	// 上方向への移動アニメーション設定
	Vector3 endPos = startPos;
	endPos.y += moveOffsetY_;
	posAnim_.Start(startPos, endPos, animDuration_, EaseType::EaseOutQuad);
	alphaAnim_.Start(0.5f, 1.0f, animDuration_, EaseType::EaseOutQuad);
}

void ValueDeltaEffect::Update(float deltaTime, const Matrix4x4& vpMat) {
	if (!isActive_) {
		return;
	}

	// アニメーションの更新
	bool isPosActive = posAnim_.Update(deltaTime, currentPos_);
	bool isAlphaActive = alphaAnim_.Update(deltaTime, currentAlpha_);

	// アニメーションが完了したら非アクティブにする
	if (!isPosActive && !isAlphaActive) {
		isActive_ = false;
		return;
	}

	Transform transform{};
	transform.position = currentPos_;
	transform.scale = {1.0f, 1.0f, 1.0f};
	text_->SetTransform(transform);
	Vector4 currentColor = baseColor_;
	currentColor.w = currentAlpha_;
	text_->SetColor(currentColor);
	text_->Update(vpMat);
}

void ValueDeltaEffect::Draw(CmdObj* cmdObj) {
	if (isActive_) {
		text_->Draw(cmdObj);
	}
}
