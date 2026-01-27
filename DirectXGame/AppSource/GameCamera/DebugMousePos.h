#pragma once
#include"Utility/Vector.h"

class DebugMousePos {
public:

	// デバック画面でのマウスの位置
	static inline Vector2 windowPos = {};

	// エンジン側のマウスの位置
	static inline Vector2 screenMousePos = {};

	// ゲーム側のマウス位置
	static inline Vector2 gameMousePos = {};

	//ゲーム画面をつかんでいるかどうか(Release時問題なくif文を超えられる用にtrue)
	static inline bool isGrabbed = true;

	//ゲーム画面上にカーソルがいるか(Release時問題なくif文を超えられる用にtrue)
	static inline bool isHovered = true;
};