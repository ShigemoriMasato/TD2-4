#pragma once
#include"Object/FontObject.h"
#include<array>

class CounterUI {
public:

	void Initialize(const std::string& fontName,const std::wstring& name,int32_t curNum,int32_t maxNum, DrawData drawData, FontLoader* fontLoader);

	void Update(int32_t curNum, int32_t maxNum);

	void Draw(Window* window, const Matrix4x4& vpMatrix);

	std::unique_ptr<FontObject> fontObject_;

private:

	// フォントロード
	FontLoader* fontLoader_ = nullptr;

	std::wstring name_;

	int32_t currenyNum_ = 0;
	int32_t maxNum_ = 0;
};