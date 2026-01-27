#include"GameOverUI.h"
#include"Utility/Easing.h"
#include"FpsCount.h"

void GameOverUI::Initialize(DrawData drawData) {
	// 描画機能の初期化
	bgSpriteObject_ = std::make_unique<SpriteObject>();
	bgSpriteObject_->Initialize(drawData, { 1280.0f,720.0f });
	bgSpriteObject_->transform_.position = { 640.0f,1080.0f,0.0f };
	bgSpriteObject_->Update();



}

void GameOverUI::Update() {

	if (inAnimation_) {
		timer_ += FpsCount::deltaTime / maxTime_;

		bgSpriteObject_->transform_.position.y = lerp(startBgPosY_, endBgPosY_, timer_, EaseType::EaseOutBounce);

		if (timer_ >= 1.0f) {
			bgSpriteObject_->transform_.position.y = endBgPosY_;
			inAnimation_ = false;
		}
	}

	

	// 背景画像の更新処理
	bgSpriteObject_->Update();
}

void GameOverUI::Draw(Window* window, const Matrix4x4& vpMatrix) {

	// 背景の描画
	bgSpriteObject_->Draw(window, vpMatrix);
}