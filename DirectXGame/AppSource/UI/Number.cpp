#include "Number.h"
#include <Utility/Easing.h>

void Number::Initialize(const std::string& fontName, DrawData drawData, FontLoader* fontLoader) {
	fontObject_ = std::make_unique<FontObject>();
	fontObject_->Initialize(fontName, L"0", drawData, fontLoader);
	transform_.scale = { 1.0f, -1.0f, 1.0f };
	transform_.rotate = { 0.0f, 0.0f, 0.0f };
	transform_.position = { 0.0f, 0.0f, 0.0f };
}

void Number::Update(float deltaTime) {
	float totalScale = 1.0f;
	for (size_t i = 0; i < scalingOrders_.size(); ++i) {
		scalingOrders_[i].scaleT_ += deltaTime * 4.0f;
		totalScale += lerp_RoundTrip(0.0f, maxScale_ / 10.0f, scalingOrders_[i].scaleT_, EaseType::EaseOutCubic);
		if (scalingOrders_[i].scaleT_ >= 1.0f) {
			scalingOrders_.erase(scalingOrders_.begin() + i);
			--i;
		}
	}
	totalScale = std::clamp(totalScale, 1.0f, maxScale_);
	uint32_t color = lerpColor(defaultColor_, highColor_, (totalScale - 1), EaseType::EaseOutCubic);
	fontObject_->fontColor_ = ConvertColor(color);
	scale_ = totalScale;

	fontObject_->transform_ = transform_;
	fontObject_->transform_.scale *= scale_;
	std::wstring number = std::to_wstring(currentNumber_);
	fontObject_->SetText(offset_ + number);

	fontObject_->Update();

	int digitCount = int(number.length());
	// 右揃え
	int advance = 41; // 文字間隔
	if (leftAlign_) {
		advance = 0;
	}
	fontObject_->transform_.position.x = transform_.position.x - (advance * (digitCount - 1) * transform_.scale.x) / 2.0f;
}

void Number::Draw(Window* window, const Matrix4x4& vpMatrix) {
	fontObject_->Draw(window, vpMatrix);
}

void Number::SetOffset(std::wstring offset) {
	offset_ = offset;
}

void Number::SetNumber(int number) {
	if (currentNumber_ != number) {
		currentNumber_ = number;
		scalingOrders_.emplace_back();
	}
}
