#pragma once
#include"Object/SpriteObject.h"
#include"Common/KeyConfig/KeyManager.h"
#include"Object/FontObject.h"
#include <functional>

#include"UI/Effect/QuotaClearEffectUI.h"
#include"UI/Effect/ConfettiEffectUI.h"

class ClearUI {
public:

	void Initialize(DrawData drawData, KeyManager* keyManager, const std::string& fontName, DrawData fontDrawData, FontLoader* fontLoader, bool hasNextMap,int florTex,int starTex);

	void Update();

	void Draw(Window* window, const Matrix4x4& vpMatrix);

	// クリアシーンの開始を宣言する
	void Start(const bool& isStart) {
		isClearStart_ = isStart;
		isAnimation_ = true;
	}

	// シーン遷移で仕様する色を取得する
	void SetSceneColor(float* fadealpha) {
		fadealpha_ = fadealpha;
	}

	bool IsClearAnimationEnd() { return isClearAnimationEnd_; }

public:

	void SetOnRetryClicked(std::function<void()> cb) { onRetryClicked_ = std::move(cb); }
	void SetOnSelectClicked(std::function<void()> cb) { onSelectClicked_ = std::move(cb); }

private:
	// 入力処理
	KeyManager* keyManager_ = nullptr;

	// 背景画像
	std::unique_ptr<SpriteObject> bgSpriteObject_;

	// フロアクリア
	std::unique_ptr<SpriteObject> florClearSpriteObject_;
	std::unique_ptr<SpriteObject> effectFlorClearSpriteObject_;
	Vector2 clearTextSize_ = { 360.0f,128.0f };

	bool isClearStart_ = false;
	bool isEnd_ = false;

	bool isClearAnimationEnd_ = false;

	float timer_ = 0.0f;

	// 背景の移動処理
	float startBgPosY_ = 1080.0f;
	float endBgPosY_ = 360.0f;

	bool isAnimation_ = true;

	// クリックコールバック
	std::function<void()> onRetryClicked_ = nullptr;
	std::function<void()> onSelectClicked_ = nullptr;

	// 選択番号
	int selectNum_ = 0;

	// キラキラエフェクト
	std::unique_ptr<QuotaClearEffectUI> quotaClearEffectUI_;

	// 吹雪演出
	std::unique_ptr<ConfettiEffectUI> lertconfettiEffectUI_;
	std::unique_ptr<ConfettiEffectUI> rightconfettiEffectUI_;

	// シーン遷移で仕様する色
	float* fadealpha_ = nullptr;

private: // 調整項目

	float maxTime_ = 2.0f;

private:

	void InUpdate();

	// スタートアニメーション
	void StartAnimation();

	// 終了アニメーション
	void EndAnimation();

};
