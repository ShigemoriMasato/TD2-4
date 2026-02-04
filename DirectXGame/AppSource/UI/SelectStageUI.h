#pragma once
#include"Object/SpriteObject.h"
#include"Object/FontObject.h"

class SelectStageUI {
public:

	void Init(DrawData drawData, const std::string& fontName,DrawData fontDrawData, FontLoader* fontLoader,int tex);

	void Update();

	void Draw(Window* window, const Matrix4x4& vpMatrix);

	bool IsAnimation() const { return isAnimation_; }

	void SetAnimation() {
		if (isAnimation_) { return; }
		isAnimation_ = true;
		timer_ = 0.0f;
	}

private:
	// フォントロード
	FontLoader* fontLoader_ = nullptr;

	// 画像データ
	std::unique_ptr<SpriteObject> spriteObject_;

	// フォントデータ
	std::unique_ptr<FontObject> fontObject_;

	// アニメーションのフラグ
	bool isAnimation_ = false;

	Vector3 defaultSize_ = {};

	float timer_ = 0.0f;

	bool isChange_ = false;

	std::wstring name_ = L"ステージ";

	// フォントの位置
	Vector3 fontPos_ =  { 497.0f,150.0f,0.0f };

private: // 調整項目

	float maxTime_ = 1.0f;
};