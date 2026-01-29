#include"OreHome.h"
#include"Utility/Easing.h"
#include"FpsCount.h"

void OreHome::Initialize(const DrawData& homeDrawData, int texture, const Vector3& pos, const float& rotY) {
	// オブジェクトの初期化
	homeObject_ = std::make_unique<DefaultObject>();
	homeObject_->Initialize(homeDrawData, texture);
	homeObject_->transform_.position = pos;
	homeObject_->transform_.rotate.y = rotY;
}

void OreHome::Update() {

	// アニメーション処理
	if (isAnimation_) {
		Animation();
	}

	// 更新処理
	homeObject_->Update();
}

void OreHome::Draw(Window* window, const Matrix4x4& vpMatrix) {

	// 描画
	homeObject_->Draw(window, vpMatrix);
}

void OreHome::Animation() {

	timer_ += FpsCount::deltaTime / maxTime_;

	if (timer_ <= 0.4f) {
		float localT = timer_ / 0.4f;

		float width = lerp(1.0f, 0.5f, localT, EaseType::EaseInCubic);
		homeObject_->transform_.scale.x = width;
		homeObject_->transform_.scale.z = width;
		homeObject_->transform_.scale.y = lerp(1.0f, 1.5f, localT, EaseType::EaseInCubic);
	} else if (timer_ <= 0.8){
		float localT = (timer_ - 0.4f) / 0.4f;

		float width = lerp(0.5f, 1.5f, localT, EaseType::EaseInCubic);
		homeObject_->transform_.scale.x = width;
		homeObject_->transform_.scale.z = width;
		homeObject_->transform_.scale.y = lerp(1.5f, 0.5f, localT, EaseType::EaseInCubic);
	} else {
		float localT = (timer_ - 0.8f) / 0.2f;

		float width = lerp(1.5f, 1.0f, localT, EaseType::EaseInCubic);
		homeObject_->transform_.scale.x = width;
		homeObject_->transform_.scale.z = width;
		homeObject_->transform_.scale.y = lerp(0.5f, 1.0f, localT, EaseType::EaseInCubic);
	}

	if (timer_ >= 1.0f) {
		isAnimation_ = false;
		timer_ = 0.0f;
	}
}