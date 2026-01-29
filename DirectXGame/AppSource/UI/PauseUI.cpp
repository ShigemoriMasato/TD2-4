#include"PauseUI.h"
#include"Utility/Easing.h"
#include"FpsCount.h"

void PauseUI::Initialize(DrawData drawData, KeyManager* keyManager, const std::string& fontName, DrawData fontDrawData, FontLoader* fontLoader) {
	keyManager_ = keyManager;

	// 描画機能の初期化
	bgSpriteObject_ = std::make_unique<SpriteObject>();
	bgSpriteObject_->Initialize(drawData, { 1280.0f,720.0f });
	bgSpriteObject_->transform_.position = { 640.0f,360.0f,0.0f };
	bgSpriteObject_->color_ = { 0.1f,0.1f,0.1f,0.9f };
	bgSpriteObject_->Update();

	// テキストの初期化
	gameOverFontObject_ = std::make_unique<FontObject>();
	gameOverFontObject_->Initialize(fontName, L"Pause Scene", fontDrawData, fontLoader);
	gameOverFontObject_->transform_.position.x = 640.0f - (static_cast<float>(gameOverFontObject_->GetTextSize()) * 48.0f * 0.5f);
	gameOverFontObject_->transform_.position.y = 220.0f;
	gameOverFontObject_->transform_.scale.x = 1.5f;
	gameOverFontObject_->transform_.scale.y = -1.5f;
	gameOverFontObject_->fontColor_ = { 0.0f,0.0f,0.0f,0.0f };
}

void PauseUI::Update() {
	//if (inAnimation_) {
	//	timer_ += FpsCount::deltaTime / maxTime_;
	//
	//	if (timer_ <= 0.8f) {
	//		float localT = timer_ / 0.8f;
	//		bgSpriteObject_->transform_.position.y = lerp(startBgPosY_, endBgPosY_, localT, EaseType::EaseOutBounce);
	//	} else {
	//		float localT = (timer_ - 0.8f) / 0.2f;
	//		float alpha = lerp(0.0f, 1.0f, localT, EaseType::EaseOutBounce);
	//
	//		// 透明度を出す
	//		gameOverFontObject_->fontColor_.w = alpha;
	//	}
	//
	//	if (timer_ >= 1.0f) {
	//		bgSpriteObject_->transform_.position.y = endBgPosY_;
	//		float alpha = 1.0f;
	//		gameOverFontObject_->fontColor_.w = alpha;
	//		inAnimation_ = false;
	//	}
	//} else {
	//	
	//}

	InUpdate();

	// 背景画像の更新処理
	bgSpriteObject_->Update();
}

void PauseUI::Draw(Window* window, const Matrix4x4& vpMatrix) {

	if (isDraw_) {
		// 背景の描画
		bgSpriteObject_->Draw(window, vpMatrix);

		//gameOverFontObject_->Draw(window, vpMatrix);
	}
}

void PauseUI::InUpdate() {

	// 操作
	auto key = keyManager_->GetKeyStates();

	if (key[Key::UpTri]) {
		selectNum_ = 0;
	}

	if (key[Key::DownTri]) {
		selectNum_ = 1;
	}

	// 決定
	if (key[Key::Decision]) {
		isDraw_ = !isDraw_;
		onRetryClicked_();
	}
}