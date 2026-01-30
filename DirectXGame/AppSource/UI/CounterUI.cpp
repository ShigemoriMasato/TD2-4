#include"CounterUI.h"
#include"FpsCount.h"
#include"Utility/Easing.h"

void CounterUI::Initialize(const std::string& fontName, int32_t curNum, int32_t maxNum, DrawData drawData, FontLoader* fontLoader) {

	// フォントローダーを取得
	fontLoader_ = fontLoader;

	// 数を取得
	currenyNum_ = curNum;
	maxNum_ = maxNum;

	// テキストの初期化
	fontObject_ = std::make_unique<FontObject>();
	fontObject_->Initialize(fontName, std::to_wstring(curNum), drawData, fontLoader);

	std::wstring s = +L"/" + std::to_wstring(maxNum);

	// テキストの初期化
	maxnumFontObject_ = std::make_unique<FontObject>();
	maxnumFontObject_->Initialize(fontName, s, drawData, fontLoader);
}

void CounterUI::Update(int32_t curNum, int32_t maxNum) {
#ifdef USE_IMGUI

	float i = 1.0f;
	if (currenyNum_ >= 10) {
		i = 2.0f;
	}

	// 位置の更新
	fontObject_->transform_.position = pos_;
	maxnumFontObject_->transform_.position = pos_ + Vector3(32.0f * i + 14.0f, 0.0f, 0.0f);
#endif

	if (curNum != currenyNum_ || maxNum != maxNum_) {

		currenyNum_ = curNum;
		maxNum_ = maxNum;

		isAnimation_ = true;

		// カウントによりテクスチャを更新
		fontObject_->UpdateCharPositions(std::to_wstring(curNum), fontLoader_);
	}

	// アニメーションの更新処理
	if (isAnimation_) {
		Animation();
	}
}

void CounterUI::Draw(Window* window, const Matrix4x4& vpMatrix) {
	
	// フォントを描画
	fontObject_->Draw(window, vpMatrix);
	maxnumFontObject_->Draw(window, vpMatrix);
}

void CounterUI::Animation() {

	timer_ += FpsCount::deltaTime / maxTime_;

	if (timer_ <= 0.5f) {
		float localT = timer_ / 0.5f;

		// 座標
		fontObject_->transform_.position.x = lerp(pos_.x, pos_.x - (32.0f * 0.5f), localT, EaseType::EaseInCubic);

		// スケール
		float width = lerp(1.0f, 1.5f, localT, EaseType::EaseInCubic);
		fontObject_->transform_.scale.x = width;
		fontObject_->transform_.scale.y = -width;
	} else {
		float localT = (timer_ - 0.5f) / 0.5f;

		// 座標
		fontObject_->transform_.position.x = lerp(pos_.x - (32.0f * 0.5f), pos_.x, localT, EaseType::EaseInCubic);

		// スケール
		float width = lerp(1.5f, 1.0f, localT, EaseType::EaseInCubic);
		fontObject_->transform_.scale.x = width;
		fontObject_->transform_.scale.y = -width;
	}

	if (timer_ >= 1.0f) {
		isAnimation_ = false;
		timer_ = 0.0f;
	}
}