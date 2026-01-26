#include"FadeTransition.h"
#include"Utility/Easing.h"
#include"FpsCount.h"

void FadeTransition::Initialize(DrawData drawData) {
	// 描画機能の初期化
	bgSpriteObject_ = std::make_unique<SpriteObject>();
	bgSpriteObject_->Initialize(drawData, { 1280.0f,720.0f });
	bgSpriteObject_->transform_.position = { 640.0f,360.0f,0.0f };
	bgSpriteObject_->Update();

	isAnimation_ = false;
	phase_ = Phase::In;
}

void FadeTransition::Update() {

	if (!isAnimation_) { return; }

	timer_ += FpsCount::deltaTime;

	switch (phase_)
	{
	case Phase::In:

		bgSpriteObject_->color_.w = lerp(1.0f, 0.0f, timer_, EaseType::EaseInOutCubic);

		if (timer_ >= 1.0f) {
			timer_ = 0.0f;
			isAnimation_ = false;
		}
		break;

	case Phase::Out:

		bgSpriteObject_->color_.w = lerp(0.0f, 1.0f, timer_, EaseType::EaseInOutCubic);

		if (timer_ >= 1.0f) {
			bgSpriteObject_->color_.w = 1.0f;
			timer_ = 0.0f;
			isFinished_ = true;
		}
		break;
	}

	bgSpriteObject_->Update();
}

void FadeTransition::Draw(Window* window, const Matrix4x4& vpMatrix) {
	if (!isAnimation_) { return; }
	// 背景の描画
	bgSpriteObject_->Draw(window, vpMatrix);
}