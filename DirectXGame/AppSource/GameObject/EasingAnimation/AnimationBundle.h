#pragma once
#include "EasingAnimation.h"

template<typename T> struct AnimationBundle {
	EasingAnimation<T> anim; // アニメーション本体
	T start;                 // 開始値
	T end;                   // 目標値
	T temp;                  // 一時値
};

/// ------------ 使い方 ------------
/// ----------------------------------------- 初期化処理 -----------------------------------------
// AnimationBundle<Vector2> scaleAnimation_; ← Vector2型の場合

// scaleAnimation_.anim = {
//     {0.0f,    0.0f   },         ← 開始値の設定
//     {100.0f, 100.0f},         ← 目標値の設定
//     1.0f, EaseType::EASEOUTBACK ← [イージングをかける時間] と [イージングのタイプの設定]
// };

// Start関数を呼んで初期化することも可能
// scaleAnimation_.anim.Start({0.0f, 0.0f}, {100.0f, 100.0f}, 1.0f, EaseType::EASEOUTBACK);


/// ----------------------------------------- 更新処理 -----------------------------------------
// scaleAnimation.anim.Update(deltaTime, scaleAnimation_.temp); ← 更新処理 引数は [デルタタイム] と [AnimationBundleの実際に動く値]
// transform_.scale.y = scaleAnimation_.temp;                   ← [AnimationBundleの実際に動く値] を 変更したい値に代入すれば完了

// ループさせたい場合
// bool playing = scaleAnimation_.anim.Update(deltaTime, scaleAnimation_.temp); ← Update関数がbool型になっていてイージングが終了していれば false していなければ true を返す
// transform_.scale.y = scaleAnimation_.temp;                                   ← 変更したい変数に値を適用
//
// ここで再初期化
// if (!playing) {
// scaleAnimation_.anim = {
//     {0.0f,    0.0f   },         ← 開始値の設定
//     {1600.0f, 1600.0f},         ← 目標値の設定
//     1.0f, EaseType::EASEOUTBACK ← [イージングをかける時間] と [イージングのタイプの設定]
// };
//}