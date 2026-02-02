#include"LogUI.h"
#include"Utility/Easing.h"
#include"FpsCount.h"
#include"Utility/Easing.h"

void LogUI::Initialize(DrawData drawData, int confTex, int effectTex) {

	logDatas_.resize(20);

	for (size_t i = 0; i < 20; ++i) {
		logDatas_[i].spriteObject_= std::make_unique<SpriteObject>();
		logDatas_[i].spriteObject_->Initialize(drawData, { 360.0f,1280.0f });
		logDatas_[i].spriteObject_->transform_.position = { -200.0f,500.0f,0.0f };
		logDatas_[i].spriteObject_->color_ = { 1.0f,1.0f,1.0f,1.0f };
		logDatas_[i].spriteObject_->SetTexture(confTex);
		logDatas_[i].spriteObject_->Update();
		// 演出
		logDatas_[i].effectObject_ = std::make_unique<SpriteObject>();
		logDatas_[i].effectObject_->Initialize(drawData, { 360.0f,1280.0f });
		logDatas_[i].effectObject_->transform_.position = { -200.0f,500.0f,0.0f };
		logDatas_[i].effectObject_->color_ = { 1.0f,1.0f,1.0f,1.0f };
		logDatas_[i].effectObject_->SetTexture(effectTex);
		logDatas_[i].effectObject_->Update();
	}
}

void LogUI::Update() {

	// ログの更新処理
	for (auto& log : logDatas_) {
		if (!log.isActive_) {
			continue;
		}

		log.timer_ += FpsCount::deltaTime / maxTime_;

		log.spriteObject_->transform_.position.x = lerp(-360.0f, 0.0f, log.timer_, EaseType::EaseInCubic);
		log.effectObject_->transform_.position.x = lerp(-100.0f, 360.0f, log.timer_, EaseType::EaseOutCubic);

		if (log.timer_ >= 1.0f) {
			log.timer_ = 0.0f;
			log.isActive_ = false;
		}

		log.effectObject_->Update();
		log.spriteObject_->Update();
	}

}

void LogUI::Draw(Window* window, const Matrix4x4& vpMatrix) {

	// logを描画
	for (auto& log : logDatas_) {
		if (!log.isActive_) {
			continue;
		}
		log.effectObject_->Draw(window, vpMatrix);
		log.spriteObject_->Draw(window, vpMatrix);
	}
}

void LogUI::AddUnitConflictLog() {

	// ログを追加
	for (auto& log : logDatas_) {
		if (log.isActive_) {
			continue;
		}

		log.isActive_ = true;
		break;
	}
}