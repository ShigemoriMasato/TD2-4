#include"GameUIManager.h"
#include"Item/OreItemStorageNum.h"

void GameUIManager::Initialize(DrawData spriteData, const std::string& fontName, DrawData fontData, FontLoader* fontLoader) {
	fontLoader_ = fontLoader;

	// ユニットの数UI
	unitCounterUI_ = std::make_unique<CounterUI>();
	unitCounterUI_->Initialize(fontName, L"ユニット :", 0, 10, fontData, fontLoader_);
	unitCounterUI_->fontObject_->transform_.position.x = 800.0f;
	unitCounterUI_->fontObject_->transform_.position.y = 128.0f;

	// 鉱石のアイテムUI
	oreItemUI_ = std::make_unique<CounterUI>();
	oreItemUI_->Initialize(fontName, L"こうせき :", OreItemStorageNum::currentOreItemNum_, OreItemStorageNum::maxOreItemNum_, fontData, fontLoader_);
	oreItemUI_->fontObject_->transform_.position.x = 800.0f;
	oreItemUI_->fontObject_->transform_.position.y = 200.0f;

	// 時間を表示するUI
	timerUI_ = std::make_unique<TimerUI>();
	timerUI_->Initialize(fontName, fontData, fontLoader_);

}

void GameUIManager::Update(const int32_t& unitNum, const int32_t& maxUnitNum) {
	// ユニットの数を更新
	unitCounterUI_->Update(unitNum, maxUnitNum);

	// 鉱石の数を更新
	oreItemUI_->Update(OreItemStorageNum::currentOreItemNum_, OreItemStorageNum::maxOreItemNum_);

	// 時間表示UIを更新
	timerUI_->Update();
}

void GameUIManager::Draw(Window* window, const Matrix4x4& vpMatrix) {

	// ユニットの数UIを描画
	unitCounterUI_->Draw(window, vpMatrix);
	// 鉱石の数UIを描画
	oreItemUI_->Draw(window, vpMatrix);

	// 時間計測表示UI
	timerUI_->Draw(window, vpMatrix);
}