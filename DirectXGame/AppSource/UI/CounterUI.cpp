#include"CounterUI.h"

void CounterUI::Initialize(const std::string& fontName, const std::wstring& name, int32_t curNum, int32_t maxNum, DrawData drawData, FontLoader* fontLoader) {

	// フォントローダーを取得
	fontLoader_ = fontLoader;

	// 数を取得
	currenyNum_ = curNum;
	maxNum_ = maxNum;

	name_ = name;

	std::wstring s = std::to_wstring(curNum) + L"/" + std::to_wstring(maxNum);

	// テキストの初期化
	fontObject_ = std::make_unique<FontObject>();
	fontObject_->Initialize(fontName,name + s, drawData, fontLoader);

}

void CounterUI::Update(int32_t curNum, int32_t maxNum) {

	if (curNum != currenyNum_ || maxNum != maxNum_) {

		currenyNum_ = curNum;
		maxNum_ = maxNum;

		// カウントによりテクスチャを更新
		std::wstring s = name_ + std::to_wstring(curNum) + L"/" + std::to_wstring(maxNum);
		fontObject_->UpdateCharPositions(s, fontLoader_);
	}
}

void CounterUI::Draw(Window* window, const Matrix4x4& vpMatrix) {
	
	// フォントを描画
	fontObject_->Draw(window, vpMatrix);
}