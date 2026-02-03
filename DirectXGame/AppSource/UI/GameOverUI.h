#pragma once
#include"Object/SpriteObject.h"
#include"Common/KeyConfig/KeyManager.h"
#include"Object/FontObject.h"
#include"UI/Number.h"
#include <functional>

class GameOverUI {
public:

	void Initialize(DrawData drawData, KeyManager* keyManager, const std::string& fontName, DrawData fontDrawData, FontLoader* fontLoader);

	void Update();

	void Draw(Window* window, const Matrix4x4& vpMatrix);

	void SetCurrentScore(int score) { score_ = score; }

public:

	void SetOnRetryClicked(std::function<void()> cb) { onRetryClicked_ = std::move(cb); }
	void SetOnSelectClicked(std::function<void()> cb) { onSelectClicked_ = std::move(cb); }

private:
	// 入力処理
	KeyManager* keyManager_ = nullptr;

	// 背景画像
	std::unique_ptr<SpriteObject> bgSpriteObject_;

	float timer_ = 0.0f;

	// 背景の移動処理
	float startBgPosY_ = 1080.0f;
	float endBgPosY_ = 360.0f;

	bool inAnimation_ = true;

	// テキスト
	std::unique_ptr<FontObject> gameOverFontObject_;

	// リトライ
	std::unique_ptr<FontObject> retryFontObject_;

	// ステージ選択へ
	std::unique_ptr<FontObject> selectFontObject_;

	// クリックコールバック
	std::function<void()> onRetryClicked_ = nullptr;
	std::function<void()> onSelectClicked_ = nullptr;

	// スコア
	std::unique_ptr<Number> scoreNumber_ = nullptr;

	int selectNum_ = 0;

	int score_ = 0;

private: // 調整項目

	float maxTime_ = 1.0f;

private:

	void InUpdate();

};