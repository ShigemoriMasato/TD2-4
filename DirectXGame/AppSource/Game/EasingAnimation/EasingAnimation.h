#pragma once

#include <../Engine/Utility/Easing.h>
#include <algorithm>

/// <summary>
/// イージングアニメーションテンプレートクラス
/// </summary>
/// <typeparam name="T">扱うデータ型</typeparam>
template<typename T> class EasingAnimation {
public:
	EasingAnimation() : isActive_(false), duration_(0.0f), time_(0.0f) {}

	EasingAnimation(T start, T end, float duration, EaseType easeType) : start_(start), end_(end), duration_(duration), time_(0.0f), easeType_(easeType), isActive_(true) {}

	// 更新処理
	bool Update(float deltaTime, T& outValue) {
		if (!isActive_) {
			return false;
		}

		// 実際にイージングをかける処理
		time_ += deltaTime;
		float t = (duration_ > 0.0f) ? std::clamp(time_ / duration_, 0.0f, 1.0f) : 1.0f;
		outValue = lerp<T>(start_, end_, t, easeType_);

		// 指定した時間が経過したらフラグをおろす
		if (time_ >= duration_) {
			isActive_ = false;
		}

		return isActive_;
	}

	// Getter
	bool GetIsActive() const { return isActive_; }

	// リセットする
	void Reset() {
		time_ = 0.0f;
		isActive_ = false;
	}

	// アニメーションを再スタートするための便利機能
	void Start(const T& start, const T& end, float duration, EaseType easeType) {
		start_ = start;
		end_ = end;
		duration_ = duration;
		easeType_ = easeType;
		time_ = 0.0f;
		isActive_ = true;
	}

private:
	T start_;           // 開始値
	T end_;             // 目標値
	float duration_;    // アニメーション時間
	float time_;        // 経過時間
	EaseType easeType_; // イージングのタイプ
	bool isActive_;     // アクティブかどうか
};