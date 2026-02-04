#pragma once
#include"Object/FontObject.h"

class StartCountUI {
public:

	void Initialize(const std::string& fontName, DrawData drawData, FontLoader* fontLoader,int florNum, const std::string& fontName1);

	void Update();

	void Draw(Window* window, const Matrix4x4& vpMatrix);

	void SetPreTime(const int& time) {
		preTime_ = time;
	}

	bool isStartAnimeEnd() const { return isStartAnimeEnd_; }

public:

	bool isStart_ = false;

private:

	FontLoader* fontLoader_ = nullptr;

	// waveテキスト
	std::unique_ptr<FontObject> waveFontObject_;

	// startテキスト
	std::unique_ptr<FontObject> startFontObject_;

	// 数テキスト
	std::unique_ptr<FontObject> numFontObject_;

	float timer_ = 0.0f;
	float startTime_ = 3.0f;
	float countTime_ = 0.4f;

	int preTime_ = 0;

	bool isChange_ = false;

	bool isStartAnimeEnd_ = false;

	bool isEndCountAnimation_ = false;

	float rotZ_ = 0.0f;

	uint32_t readySH_ = 0;
	uint32_t startSH_ = 0;

	bool isPlayReady_ = false;

private:

	void StartAnimation();

	void EndAnimation();

	/// <summary>
	/// 値を登録する
	/// </summary>
	void RegisterDebugParam();

	/// <summary>
	/// 値を適応する
	/// </summary>
	void ApplyDebugParam();
};