#include"SelectStageUI.h"
#include"Utility/Easing.h"
#include"FpsCount.h"
#include"SelectStageNum.h"

void SelectStageUI::Init(DrawData drawData, const std::string& fontName, DrawData fontDrawData, FontLoader* fontLoader, int tex) {

	fontLoader_ = fontLoader;

	// 描画機能の初期化
	spriteObject_ = std::make_unique<SpriteObject>();
	spriteObject_->Initialize(drawData,{500.0f,140.0f});
	spriteObject_->transform_.position.x = 640.0f;
	spriteObject_->transform_.position.y = 128.0f;
	spriteObject_->SetTexture(tex);

	// デフォルトのサイズを設定
	defaultSize_ = spriteObject_->transform_.scale;

	fontPos_.x = 615.0f - (32.0f * 5 * 0.5f);

	// テキストの初期化
	fontObject_ = std::make_unique<FontObject>();
	fontObject_->Initialize(fontName, name_ + std::to_wstring(SelectStageNum::num_), fontDrawData, fontLoader);
	fontObject_->transform_.position.x = fontPos_.x;
	fontObject_->transform_.position.y = fontPos_.y;
	fontObject_->fontColor_ = { 1.0f,1.0f,1.0f,1.0f };
}

void SelectStageUI::Update() {

	if (isAnimation_) {

		timer_ += FpsCount::deltaTime / maxTime_;

		if (timer_ <= 0.5f) {

			float localT = timer_ / 0.5f;

			spriteObject_->transform_.scale.x = lerp(defaultSize_.x, 0.0f,localT, EaseType::EaseInCubic);
			spriteObject_->transform_.scale.y = lerp(defaultSize_.y, 0.0f, localT, EaseType::EaseInCubic);

			fontObject_->transform_.scale.x = lerp(1.0f, 0.0f, localT, EaseType::EaseInCubic);
			fontObject_->transform_.scale.y = lerp(-1.0f, 0.0f, localT, EaseType::EaseInCubic);
			fontObject_->transform_.position.x = lerp(fontPos_.x, fontPos_.x + (32.0f * 5 * 0.5f), localT, EaseType::EaseInCubic);
			fontObject_->transform_.position.y = lerp(fontPos_.y, fontPos_.y - 32.0f, localT, EaseType::EaseInCubic);

		} else {

			if (!isChange_) {
				isChange_ = true;

				if (SelectStageNum::num_ >= 3) {
					// ステージ番号を切り替え
					std::wstring s = L"エンドレス";
					fontObject_->UpdateCharPositions(s, fontLoader_);
				} else {
					// ステージ番号を切り替え
					std::wstring s = name_ + std::to_wstring(SelectStageNum::num_);
					fontObject_->UpdateCharPositions(s, fontLoader_);
				}
			}

			float localT = (timer_ - 0.5f) / 0.5f;
			spriteObject_->transform_.scale.x = lerp(0.0f, defaultSize_.x, localT, EaseType::EaseOutCubic);
			spriteObject_->transform_.scale.y = lerp(0.0f, defaultSize_.y, localT, EaseType::EaseOutCubic);

			fontObject_->transform_.scale.x = lerp(0.0f, 1.0f, localT, EaseType::EaseInCubic);
			fontObject_->transform_.scale.y = lerp(0.0f, -1.0f, localT, EaseType::EaseInCubic);
			fontObject_->transform_.position.x = lerp(fontPos_.x + (32.0f * 5 * 0.5f), fontPos_.x, localT, EaseType::EaseInCubic);
			fontObject_->transform_.position.y = lerp(fontPos_.y - 32.0f, fontPos_.y, localT, EaseType::EaseInCubic);
		}

		if (timer_ >= 1.0f) {
			isChange_ = false;
			isAnimation_ = false;
			fontObject_->transform_.position.x = fontPos_.x;
			spriteObject_->transform_.scale.x = defaultSize_.x;
			spriteObject_->transform_.scale.y = defaultSize_.y;
			fontObject_->transform_.scale.x = 1.0f;
			fontObject_->transform_.scale.y = -1.0f;
		}
	}

	// 更新処理
	spriteObject_->Update();
}

void SelectStageUI::Draw(Window* window, const Matrix4x4& vpMatrix) {

	// ステージロゴを描画
	spriteObject_->Draw(window,vpMatrix);

	// フォントを描画
	fontObject_->Draw(window, vpMatrix);
}