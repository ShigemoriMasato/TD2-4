#include"LogUI.h"
#include"Utility/Easing.h"
#include"FpsCount.h"
//#include"Utility/Easing.h"

void LogUI::Initialize(DrawData drawData, int confTex, int effectTex, int deathTex) {

	logDatas_.resize(20);

	for (size_t i = 0; i < 20; ++i) {
		logDatas_[i].spriteObject_= std::make_unique<SpriteObject>();
		logDatas_[i].spriteObject_->Initialize(drawData, { 360.0f,120.0f });
		logDatas_[i].spriteObject_->transform_.position = { -200.0f,500.0f,0.0f };
		logDatas_[i].spriteObject_->color_ = { 1.0f,1.0f,1.0f,1.0f };
		logDatas_[i].spriteObject_->SetTexture(confTex);
		logDatas_[i].spriteObject_->Update();
		// 演出
		logDatas_[i].effectObject_ = std::make_unique<SpriteObject>();
		logDatas_[i].effectObject_->Initialize(drawData, { 128.0f,160.0f });
		logDatas_[i].effectObject_->transform_.position = { -200.0f,500.0f,0.0f };
		logDatas_[i].effectObject_->color_ = { 1.0f,1.0f,1.0f,1.0f };
		logDatas_[i].effectObject_->SetTexture(effectTex);
		logDatas_[i].effectObject_->Update();
	}

	conflTex_ = confTex;
	deathTex_ = deathTex;
}

void LogUI::Update() {

	// ログの更新処理
	for (auto& log : logDatas_) {
		if (!log.isActive_) {
			continue;
		}

		if (log.inAnimation) {

			log.timer_ += FpsCount::deltaTime / maxTime_;

			if (log.timer_ <= 0.5f) {

			}else if (log.timer_ <= 0.8f) {
				float localT = (log.timer_ - 0.5f) / 0.3f;
				log.effectObject_->transform_.position.x = lerp(-128.0f, 360.0f, localT, EaseType::EaseInCubic);
				log.effectObject_->transform_.scale.x = lerp(128.0f, 96.0f, localT, EaseType::EaseInCubic);
				log.effectObject_->color_.w = lerp(1.0f, 0.2f, localT, EaseType::EaseOutCubic);
			} else if (log.timer_ <= 1.0f) {
				float localT = (log.timer_ - 0.8f) / 0.2f;
				log.effectObject_->color_.w = lerp(0.2f, 0.0f, localT, EaseType::EaseOutCubic);
				log.effectObject_->transform_.scale.x = lerp(96.0f, 0.0f, localT, EaseType::EaseInCubic);
			}

			if (log.timer_ <= 0.5f) {
				float localT = log.timer_ / 0.5f;
				log.spriteObject_->transform_.position.x = lerp(-360.0f, 220.0f, localT, EaseType::EaseInCubic);
			}

			if (log.timer_ >= 1.0f) {
				log.timer_ = 0.0f;
				log.effectObject_->color_.w = 0.0f;
				log.inAnimation = false;
			}
		} else {

			log.timer_ += FpsCount::deltaTime / 2.0f;

			if (log.timer_ >= 0.5f) {
				float localT = (log.timer_ - 0.5f) / 0.5f;
				log.spriteObject_->color_.w = lerp(1.0f, 0.0f, localT, EaseType::EaseOutCubic);
			}

			if (log.timer_ >= 1.0f) {
				log.spriteObject_->color_.w = 1.0f;
				log.isActive_ = false;
			}
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
		log.spriteObject_->Draw(window, vpMatrix);
		log.effectObject_->Draw(window, vpMatrix);
	}
}

void LogUI::AddUnitConflictLog() {

	// ログ1つ分の高さ＋隙間
	const float kStepY = 150.0f;

	// 表示中のログをすべて上にずらす
	for (auto& log : logDatas_) {
		if (log.isActive_) {
			log.spriteObject_->transform_.position.y -= kStepY;
			log.effectObject_->transform_.position.y -= kStepY;
		}
	}

	// 新しいログを定位置（Y=500）に出現させる
	for (auto& log : logDatas_) {
		if (log.isActive_) {
			continue;
		}

		log.isActive_ = true;
		log.inAnimation = true;
		log.timer_ = 0.0f;

		log.spriteObject_->transform_.position.y = 500.0f;
		log.spriteObject_->SetTexture(conflTex_);
		log.effectObject_->transform_.position.y = 500.0f;

		// エフェクト等の初期化
		log.effectObject_->color_.w = 1.0f;
		log.effectObject_->transform_.position.x = -128.0f;
		log.effectObject_->Update();
		break;
	}
}

void LogUI::AddUnitDeathLog() {

	// ログ1つ分の高さ＋隙間
	const float kStepY = 150.0f;

	// 表示中のログをすべて上にずらす
	for (auto& log : logDatas_) {
		if (log.isActive_) {
			log.spriteObject_->transform_.position.y -= kStepY;
			log.effectObject_->transform_.position.y -= kStepY;
		}
	}

	// 新しいログを定位置（Y=500）に出現させる
	for (auto& log : logDatas_) {
		if (log.isActive_) {
			continue;
		}

		log.isActive_ = true;
		log.inAnimation = true;
		log.timer_ = 0.0f;

		log.spriteObject_->transform_.position.y = 500.0f;
		log.spriteObject_->SetTexture(deathTex_);
		log.effectObject_->transform_.position.y = 500.0f;

		// エフェクト等の初期化
		log.effectObject_->color_.w = 1.0f;
		log.effectObject_->transform_.position.x = -128.0f;
		log.effectObject_->Update();
		break;
	}
}