#include"GameOverUI.h"
#include"Utility/Easing.h"
#include"FpsCount.h"

void GameOverUI::Initialize(DrawData drawData, KeyManager* keyManager, const std::string& fontName, DrawData fontDrawData, FontLoader* fontLoader) {
	keyManager_ = keyManager;

	// 描画機能の初期化
	bgSpriteObject_ = std::make_unique<SpriteObject>();
	bgSpriteObject_->Initialize(drawData, { 1280.0f,720.0f });
	bgSpriteObject_->transform_.position = { 640.0f,1080.0f,0.0f };
	bgSpriteObject_->color_ = { 0.1f,0.1f,0.1f,0.9f };
	bgSpriteObject_->Update();

	// テキストの初期化
	gameOverFontObject_ = std::make_unique<FontObject>();
	gameOverFontObject_->Initialize(fontName, L"Game Over", fontDrawData, fontLoader);
	gameOverFontObject_->transform_.position.x = 640.0f - (static_cast<float>(gameOverFontObject_->GetTextSize()) * 48.0f * 0.5f);
	gameOverFontObject_->transform_.position.y = 220.0f;
	gameOverFontObject_->transform_.scale.x = 1.5f;
	gameOverFontObject_->transform_.scale.y = -1.5f;
	gameOverFontObject_->fontColor_ = { 0.0f,0.0f,0.0f,0.0f };

	retryFontObject_ = std::make_unique<FontObject>();
	retryFontObject_->Initialize(fontName, L"Retry", fontDrawData, fontLoader);
	retryFontObject_->transform_.position.x  = 640.0f - (static_cast<float>(retryFontObject_->GetTextSize()) * 32.0f * 0.5f);
	retryFontObject_->transform_.position.y = 415.0f;

	selectFontObject_ = std::make_unique<FontObject>();
	selectFontObject_->Initialize(fontName, L"SelectStage", fontDrawData, fontLoader);
	selectFontObject_->transform_.position.x = 640.0f - (static_cast<float>(selectFontObject_->GetTextSize()) * 32.0f * 0.5f);
	selectFontObject_->transform_.position.y = 560.0f;

	retryFontObject_->fontColor_ = {1.0f,1.0f,1.0f,0.0f};
	selectFontObject_->fontColor_ = { 0.5f,0.5f,0.5f,0.0f };

	scoreNumber_ = std::make_unique<Number>();
	scoreNumber_->Initialize(fontName, fontDrawData, fontLoader);
	scoreNumber_->SetOffset(L"Score: ");
	scoreNumber_->SetColor(0x88888800, 0x88888800);
	scoreNumber_->SetMaxScale(1.0f);
	scoreNumber_->transform_.position = { 430.0f, 320.0f, 0.0f };
	scoreNumber_->transform_.scale = { 0.8f, -0.8f, 1.0f };
}

void GameOverUI::Update() {
	scoreNumber_->SetNumber(score_);
	scoreNumber_->Update(FpsCount::deltaTime);

	if (inAnimation_) {
		timer_ += FpsCount::deltaTime / maxTime_;

		if (timer_ <= 0.8f) {
			float localT = timer_ / 0.8f;
			bgSpriteObject_->transform_.position.y = lerp(startBgPosY_, endBgPosY_, localT, EaseType::EaseOutBounce);
		} else {
			float localT = (timer_ - 0.8f) / 0.2f;
			float alpha = lerp(0.0f, 1.0f, localT, EaseType::EaseOutBounce);

			// 透明度を出す
			gameOverFontObject_->fontColor_.w = alpha;
			retryFontObject_->fontColor_.w = alpha;
			selectFontObject_->fontColor_.w = alpha;
			scoreNumber_->SetColor(ConvertColor(Vector4({ 0.5f, 0.5f, 0.5f, alpha })), ConvertColor(Vector4({ 0.5f, 0.5f, 0.5f, alpha })));
		}

		if (timer_ >= 1.0f) {
			bgSpriteObject_->transform_.position.y = endBgPosY_;
			float alpha = 1.0f;
			gameOverFontObject_->fontColor_.w = alpha;
			retryFontObject_->fontColor_.w = alpha;
			selectFontObject_->fontColor_.w = alpha;
			inAnimation_ = false;
		}
	} else {
		InUpdate();
	}

	// 背景画像の更新処理
	bgSpriteObject_->Update();
}

void GameOverUI::Draw(Window* window, const Matrix4x4& vpMatrix) {

	// 背景の描画
	bgSpriteObject_->Draw(window, vpMatrix);

	gameOverFontObject_->Draw(window, vpMatrix);

	//retryFontObject_->Draw(window, vpMatrix);

	selectFontObject_->Draw(window, vpMatrix);

	scoreNumber_->Draw(window, vpMatrix);
}

void GameOverUI::InUpdate() {

	// 操作
	auto key = keyManager_->GetKeyStates();

	if (key[Key::UpTri]) {
		selectNum_ = 0;
		retryFontObject_->fontColor_ = {1.0f,1.0f,1.0f,1.0f};
		selectFontObject_->fontColor_ = {0.5f,0.5f,0.5f,1.0f};
	}

	if (key[Key::DownTri]) {
		selectNum_ = 1;
		retryFontObject_->fontColor_ = { 0.5f,0.5f,0.5f,1.0f };
		selectFontObject_->fontColor_ = { 1.0f,1.0f,1.0f,1.0f };
	}

	selectNum_ = 1;
	retryFontObject_->fontColor_ = { 0.5f,0.5f,0.5f,1.0f };
	selectFontObject_->fontColor_ = { 1.0f,1.0f,1.0f,1.0f };

	// 決定
	if (key[Key::Decision]) {

		if (selectNum_ == 0) {
			// やり直す
			onRetryClicked_();
		} else {
			// ステージ選択へ
			onSelectClicked_();
		}
	}
}