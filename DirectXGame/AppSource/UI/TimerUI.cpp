#include"TimerUI.h"
#include"TimeLimit.h"

void TimerUI::Initialize(const std::string& fontName, DrawData drawData, FontLoader* fontLoader) {
	// フォントローダーを取得
	fontLoader_ = fontLoader;

	// 番号を登録
	numText_[0] = L"0";
	numText_[1] = L"1";
	numText_[2] = L"2";
	numText_[3] = L"3";
	numText_[4] = L"4";
	numText_[5] = L"5";
	numText_[6] = L"6";
	numText_[7] = L"7";
	numText_[8] = L"8";
	numText_[9] = L"9";

	// テキストの初期化
	for (int i = 0; i < numSprites_.size(); ++i) {
		numSprites_[i] = std::make_unique<FontObject>();
		numSprites_[i]->Initialize(fontName, numText_[0], drawData, fontLoader);
		if (i <= 1) {
			numSprites_[i]->transform_.position.x = pos_.x + i * 32.0f;
		} else {
			numSprites_[i]->transform_.position.x = pos_.x + i * 32.0f + 54.0f;
		}
		numSprites_[i]->transform_.position.y = pos_.y;
	}
}

void TimerUI::Update() {

	// 時間の更新処理
	for (int i = 0; i < numSprites_.size(); ++i) {
		if (i == 1) {
			numSprites_[i]->UpdateCharPositions(numText_[TimeLimit::numbers[i]] + L" : ", fontLoader_);
		} else {
			numSprites_[i]->UpdateCharPositions(numText_[TimeLimit::numbers[i]], fontLoader_);
		}	
	}
}

void TimerUI::Draw(Window* window, const Matrix4x4& vpMatrix) {
	// 数字を描画する
	for (int i = 0; i < numSprites_.size(); ++i) {
		numSprites_[i]->Draw(window, vpMatrix);
	}
}