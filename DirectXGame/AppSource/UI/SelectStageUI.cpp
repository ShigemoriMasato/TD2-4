#include"SelectStageUI.h"
#include"Utility/Easing.h"
#include"FpsCount.h"

void SelectStageUI::Init(DrawData drawData) {

	// 描画機能の初期化
	spriteObject_ = std::make_unique<SpriteObject>();
	spriteObject_->Initialize(drawData,{512.0f,64.0f});
	spriteObject_->transform_.position.x = 640.0f;
	spriteObject_->transform_.position.y = 128.0f;

	// デフォルトのサイズを設定
	defaultSize_ = spriteObject_->transform_.scale;
}

void SelectStageUI::Update() {

	if (isAnimation_) {

		timer_ += FpsCount::deltaTime / maxTime_;

		if (timer_ <= 0.5f) {

			float localT = timer_ / 0.5f;

			spriteObject_->transform_.scale.x = lerp(defaultSize_.x, 0.0f,localT, EaseType::EaseInCubic);
			spriteObject_->transform_.scale.y = lerp(defaultSize_.y, 0.0f, localT, EaseType::EaseInCubic);
		} else {

			if (!isChange_) {
				isChange_ = true;

				// ここでテクスチャ切り替え
			}

			float localT = (timer_ - 0.5f) / 0.5f;
			spriteObject_->transform_.scale.x = lerp(0.0f, defaultSize_.x, localT, EaseType::EaseOutCubic);
			spriteObject_->transform_.scale.y = lerp(0.0f, defaultSize_.y, localT, EaseType::EaseOutCubic);
		}

		if (timer_ >= 1.0f) {
			isChange_ = false;
			isAnimation_ = false;
			spriteObject_->transform_.scale.x = defaultSize_.x;
			spriteObject_->transform_.scale.y = defaultSize_.y;
		}
	}

	// 更新処理
	spriteObject_->Update();
}

void SelectStageUI::Draw(Window* window, const Matrix4x4& vpMatrix) {

	// ステージロゴを描画
	spriteObject_->Draw(window,vpMatrix);
}