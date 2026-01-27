#pragma once
#include"Object/FontObject.h"
#include<array>

class TimerUI {
public:

	void Initialize(const std::string& fontName, DrawData drawData, FontLoader* fontLoader);

	void Update();

	void Draw(Window* window, const Matrix4x4& vpMatrix);

public:

	Vector3 pos_ = {500.0f,128.0f,0.0f};

private:

	// フォントロード
	FontLoader* fontLoader_ = nullptr;

	// 四桁の番号
	std::array<std::unique_ptr<FontObject>, 4> numSprites_;

	// 番号テキスト
	std::wstring numText_[10];
};