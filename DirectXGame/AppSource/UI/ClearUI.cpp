#include"ClearUI.h"
#include"Utility/Easing.h"
#include"FpsCount.h"

#ifdef USE_IMGUI
#include <imgui/imgui.h>
#endif

void ClearUI::Initialize(DrawData drawData, KeyManager* keyManager, const std::string& fontName, DrawData fontDrawData, FontLoader* fontLoader, bool hasNextMap, int florTex, int starTex) {
	keyManager_ = keyManager;

	// 描画機能の初期化
	bgSpriteObject_ = std::make_unique<SpriteObject>();
	bgSpriteObject_->Initialize(drawData, { 1280.0f,720.0f });
	bgSpriteObject_->transform_.position = { 640.0f,360.0f,0.0f };
	bgSpriteObject_->color_ = { 0.0f,0.0f,0.0f,0.0f };
	bgSpriteObject_->Update();

	// フロアクリアの文字を初期化
	clearTextSize_ = { 480.0f,160.0f };
	florClearSpriteObject_ = std::make_unique<SpriteObject>();
	florClearSpriteObject_->Initialize(drawData, { clearTextSize_.x,clearTextSize_.y });
	florClearSpriteObject_->transform_.position = { 658.0f,210.0f,0.0f };
	florClearSpriteObject_->color_ = { 1.0f,1.0f,1.0f,0.0f };
	florClearSpriteObject_->SetTexture(florTex);
	florClearSpriteObject_->Update();
	// 演出用のクリア文字
	effectFlorClearSpriteObject_ = std::make_unique<SpriteObject>();
	effectFlorClearSpriteObject_->Initialize(drawData, { clearTextSize_.x,clearTextSize_.y });
	effectFlorClearSpriteObject_->transform_.position = { 658.0f,210.0f,0.0f };
	effectFlorClearSpriteObject_->color_ = { 1.0f,1.0f,1.0f,0.0f };
	effectFlorClearSpriteObject_->SetTexture(florTex);
	effectFlorClearSpriteObject_->Update();

	// キラキラエフェクトを描画
	quotaClearEffectUI_ = std::make_unique<QuotaClearEffectUI>();
	quotaClearEffectUI_->Initialize(drawData, starTex);
	quotaClearEffectUI_->pos_ = { 640.0f,208.0f,0.0f };
	quotaClearEffectUI_->range_ = { 267.0f,100.0f };
	quotaClearEffectUI_->isLoop_ = false;
	isAnimation_ = false;

	// クラッカー
	lertconfettiEffectUI_ = std::make_unique<ConfettiEffectUI>();
	lertconfettiEffectUI_->Initialize(drawData,0);
	lertconfettiEffectUI_->pos_ = { 0,720,0 };
	lertconfettiEffectUI_->isLoop_ = true;

	rightconfettiEffectUI_ = std::make_unique<ConfettiEffectUI>();
	rightconfettiEffectUI_->Initialize(drawData, 0);
	rightconfettiEffectUI_->pos_ = { 1280,720,0 };
	rightconfettiEffectUI_->isLoop_ = true;

	isClearStart_ = false;

	// 演出の更新処理
	quotaClearEffectUI_->Update();
	lertconfettiEffectUI_->Update();
	rightconfettiEffectUI_->Update();

	// 更新処理
	florClearSpriteObject_->Update();
	effectFlorClearSpriteObject_->Update();
	// 背景画像の更新処理
	bgSpriteObject_->Update();

	quotaClearEffectUI_->isLoop_ = false;
}

void ClearUI::Update() {

	if (!isClearStart_) { return; }

	if (isAnimation_) {
		
		if (isEnd_) {
			// エンドアニメーション
			timer_ += FpsCount::deltaTime / 1.0f;

			*fadealpha_ = lerp(0.0f, 1.0f, timer_, EaseType::EaseInCubic);

			if (timer_ >= 1.0f) {
				isClearAnimationEnd_ = true;
				//if (selectNum_ == 0) {
				//	// やり直す
				//	onRetryClicked_();
				//} else {
				//	// ステージ選択へ
				//	onSelectClicked_();
				//}
			}
		} else {
			// スタートアニメーション
			StartAnimation();
		}
	
	} 

	// 演出の更新処理
	quotaClearEffectUI_->Update();
	lertconfettiEffectUI_->Update();
	rightconfettiEffectUI_->Update();

	// 更新処理
	florClearSpriteObject_->Update();
	effectFlorClearSpriteObject_->Update();
	// 背景画像の更新処理
	bgSpriteObject_->Update();

}

void ClearUI::Draw(Window* window, const Matrix4x4& vpMatrix) {
	if (!isClearStart_) { return; }
	// 背景の描画
	bgSpriteObject_->Draw(window, vpMatrix);

	// クリア文字を描画
	florClearSpriteObject_->Draw(window, vpMatrix);
	// クリア文字の演出を描画
	effectFlorClearSpriteObject_->Draw(window, vpMatrix);

	// 演出の描画
	quotaClearEffectUI_->Draw(window, vpMatrix);
	lertconfettiEffectUI_->Draw(window, vpMatrix);
	rightconfettiEffectUI_->Draw(window, vpMatrix);

	//if (retryFontObject_) {
	//	retryFontObject_->Draw(window, vpMatrix);
	//}
	//
	//selectFontObject_->Draw(window, vpMatrix);
}

void ClearUI::InUpdate() {

	// 操作
	auto key = keyManager_->GetKeyStates();

	// 決定
	if (key[Key::Decision]) {

		timer_ = 0.0f;
		isEnd_ = true;
	}
}

void ClearUI::StartAnimation() {
	timer_ += FpsCount::deltaTime / maxTime_;

	if (timer_ <= 0.2f) {
		// 背景のフェード
		float localT = timer_ / 0.2f;
		bgSpriteObject_->color_.w = lerp(0.0f, 0.9f, localT, EaseType::EaseInOutCubic);

	} else if (timer_ <= 0.4f) {
		float localT = (timer_ - 0.2f) / 0.2f;
		// 透明度
		florClearSpriteObject_->color_.w = lerp(0.0f, 1.0f, localT, EaseType::EaseInCubic);
		// x軸につぶれる
		florClearSpriteObject_->transform_.scale.x = lerp(clearTextSize_.x, clearTextSize_.x - 150.0f, localT, EaseType::EaseInCubic);
		florClearSpriteObject_->transform_.scale.y = lerp(clearTextSize_.y, clearTextSize_.y + 100.0f, localT, EaseType::EaseInCubic);

	} else if (timer_ <= 0.7f) {
		float localT = (timer_ - 0.4f) / 0.3f;
		// y軸につぶれる
		florClearSpriteObject_->transform_.scale.x = lerp(clearTextSize_.x - 150.0f, clearTextSize_.x + 150.0f, localT, EaseType::EaseInCubic);
		florClearSpriteObject_->transform_.scale.y = lerp(clearTextSize_.y + 100.0f, clearTextSize_.y - 100.0f, localT, EaseType::EaseInCubic);

	} else {
		float localT = (timer_ - 0.7f) / 0.3f;
		// 元に戻る
		florClearSpriteObject_->transform_.scale.x = lerp(clearTextSize_.x + 150.0f, clearTextSize_.x, localT, EaseType::EaseInCubic);
		florClearSpriteObject_->transform_.scale.y = lerp(clearTextSize_.y - 100.0f, clearTextSize_.y, localT, EaseType::EaseInCubic);
	}

	// クリア文字の演出
	if (timer_ <= 0.4f) {

	} else if (timer_ <= 0.5f) {
		float localT = (timer_ - 0.4f) / 0.1f;
		effectFlorClearSpriteObject_->color_.w = lerp(0.0f, 0.6f, localT, EaseType::EaseOutCubic);
	} else if (timer_ <= 0.8f) {
		float localT = (timer_ - 0.5f) / 0.3f;
		effectFlorClearSpriteObject_->transform_.scale.x = lerp(0.0f, clearTextSize_.x * 1.8f, localT, EaseType::EaseInCubic);
		effectFlorClearSpriteObject_->transform_.scale.y = lerp(0.0f, clearTextSize_.y * 1.8f, localT, EaseType::EaseInCubic);
	} else if (timer_ <= 1.0f) {
		float localT = (timer_ - 0.8f) / 0.2f;
		effectFlorClearSpriteObject_->transform_.scale.x = lerp(clearTextSize_.x * 1.8f, 0.0f, localT, EaseType::EaseInCubic);
		effectFlorClearSpriteObject_->transform_.scale.y = lerp(clearTextSize_.y * 1.8f, 0.0f, localT, EaseType::EaseInCubic);
		effectFlorClearSpriteObject_->color_.w = lerp(0.6f, 0.0f, localT, EaseType::EaseOutCubic);
	}

	// 次のシーン
	if (timer_ <= 0.7f) {

	}else if (timer_ <= 0.9f) {
		float localT = (timer_ - 0.7f) / 0.2f;
	}

	if (timer_ <= 0.8f) {

	} else if (timer_ <= 1.0f) {
	}

	// 終了
	if (timer_ >= 1.0f) {
		timer_ = 0.0f;
		isAnimation_ = false;
		effectFlorClearSpriteObject_->color_.w = 0.0f;
		effectFlorClearSpriteObject_->transform_.scale.x = clearTextSize_.x;
		effectFlorClearSpriteObject_->transform_.scale.y = clearTextSize_.y;

		quotaClearEffectUI_->isLoop_ = true;

		isEnd_ = true;
		isAnimation_ = true;
	}
}

