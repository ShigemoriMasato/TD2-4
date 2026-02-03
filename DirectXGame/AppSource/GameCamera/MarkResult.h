#pragma once
#include"Utility/Vector.h"

struct MarkerResult {
	Vector2 position; // 画面上の描画位置
	float rotation;   // マーカーの回転角度（ラジアン）
	bool isVisible;   // 画面内にいるかどうか（画面内なら表示しない、などの判定用）
};
