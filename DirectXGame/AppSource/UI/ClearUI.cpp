#include"ClearUI.h"
#include"Utility/Easing.h"
#include"FpsCount.h"

void ClearUI::Initialize(DrawData drawData, KeyManager* keyManager, const std::string& fontName, DrawData fontDrawData, FontLoader* fontLoader, bool hasNextMap) {
	keyManager_ = keyManager;

	// 描画機能の初期化
	bgSpriteObject_ = std::make_unique<SpriteObject>();
	bgSpriteObject_->Initialize(drawData, { 1280.0f,720.0f });
	bgSpriteObject_->transform_.position = { 640.0f,1080.0f,0.0f };
	bgSpriteObject_->color_ = { 0.1f,0.1f,0.1f,0.9f };
	bgSpriteObject_->Update();

	// テキストの初期化
	clearFontObject_ = std::make_unique<FontObject>();
	clearFontObject_->Initialize(fontName, L"Game Clear", fontDrawData, fontLoader);
	clearFontObject_->transform_.position.x = 640.0f - (static_cast<float>(clearFontObject_->GetTextSize()) * 48.0f * 0.5f);
	clearFontObject_->transform_.position.y = 220.0f;
	clearFontObject_->transform_.scale.x = 1.5f;
	clearFontObject_->transform_.scale.y = -1.5f;
	clearFontObject_->fontColor_ = { 1.0f,1.0f,0.0f,0.0f };

	if (hasNextMap) {
		retryFontObject_ = std::make_unique<FontObject>();
		retryFontObject_->Initialize(fontName, L"Next", fontDrawData, fontLoader);
		retryFontObject_->transform_.position.x = 640.0f - (static_cast<float>(retryFontObject_->GetTextSize()) * 32.0f * 0.5f);
		retryFontObject_->transform_.position.y = 415.0f;
	}

	selectFontObject_ = std::make_unique<FontObject>();
	selectFontObject_->Initialize(fontName, L"SelectStage", fontDrawData, fontLoader);
	selectFontObject_->transform_.position.x = 640.0f - (static_cast<float>(selectFontObject_->GetTextSize()) * 32.0f * 0.5f);
	selectFontObject_->transform_.position.y = 560.0f;

	if (hasNextMap) {
		retryFontObject_->fontColor_ = { 1.0f,1.0f,1.0f,0.0f };
	}
	selectFontObject_->fontColor_ = { 0.5f,0.5f,0.5f,0.0f };
}

void ClearUI::Update() {
	if (inAnimation_) {
		timer_ += FpsCount::deltaTime / maxTime_;

		if (timer_ <= 0.8f) {
			float localT = timer_ / 0.8f;
			bgSpriteObject_->transform_.position.y = lerp(startBgPosY_, endBgPosY_, localT, EaseType::EaseOutBounce);
		} else {
			float localT = (timer_ - 0.8f) / 0.2f;
			float alpha = lerp(0.0f, 1.0f, localT, EaseType::EaseOutBounce);

			// 透明度を出す
			clearFontObject_->fontColor_.w = alpha;
			if (retryFontObject_) {
				retryFontObject_->fontColor_.w = alpha;
			}
			selectFontObject_->fontColor_.w = alpha;
		}

		if (timer_ >= 1.0f) {
			bgSpriteObject_->transform_.position.y = endBgPosY_;
			float alpha = 1.0f;
			clearFontObject_->fontColor_.w = alpha;
			if (retryFontObject_) {
				retryFontObject_->fontColor_.w = alpha;
			}
			selectFontObject_->fontColor_.w = alpha;
			inAnimation_ = false;
		}
	} else {
		InUpdate();
	}

	// 背景画像の更新処理
	bgSpriteObject_->Update();
}

void ClearUI::Draw(Window* window, const Matrix4x4& vpMatrix) {

	// 背景の描画
	bgSpriteObject_->Draw(window, vpMatrix);

	clearFontObject_->Draw(window, vpMatrix);

	if (retryFontObject_) {
		retryFontObject_->Draw(window, vpMatrix);
	}

	selectFontObject_->Draw(window, vpMatrix);
}

void ClearUI::InUpdate() {

	// 操作
	auto key = keyManager_->GetKeyStates();

	if (retryFontObject_) {
		if (key[Key::UpTri]) {
			selectNum_ = 0;
			retryFontObject_->fontColor_ = { 1.0f,1.0f,1.0f,1.0f };
			selectFontObject_->fontColor_ = { 0.5f,0.5f,0.5f,1.0f };
		}

		if (key[Key::DownTri]) {
			selectNum_ = 1;
			retryFontObject_->fontColor_ = { 0.5f,0.5f,0.5f,1.0f };
			selectFontObject_->fontColor_ = { 1.0f,1.0f,1.0f,1.0f };
		}
	} else {
		selectNum_ = 1;
		selectFontObject_->fontColor_ = { 1.0f,1.0f,1.0f,1.0f };
	}

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