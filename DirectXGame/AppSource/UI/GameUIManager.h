#pragma once
#include"Object/FontObject.h"
#include<array>

#include"UI/CounterUI.h"
#include"UI/TimerUI.h"

class GameUIManager {
public:

	void Initialize(DrawData spriteData,const std::string& fontName, DrawData fontData, FontLoader* fontLoader);

	void Update(const int32_t& unitNum,const int32_t& maxUnitNum);

	void Draw(Window* window, const Matrix4x4& vpMatrix);

private:

	// フォントロード
	FontLoader* fontLoader_ = nullptr;

	// 計測時間をだすUI
	std::unique_ptr<TimerUI> timerUI_;

	// ユニットの数UI
	std::unique_ptr<CounterUI> unitCounterUI_;
	// 鉱石の数UI
	std::unique_ptr<CounterUI> oreItemUI_;
};