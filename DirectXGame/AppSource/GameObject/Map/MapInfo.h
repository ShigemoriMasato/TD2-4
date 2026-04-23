#pragma once

/// @brief マップに関する情報をまとめる構造体
struct MapInfo {
	float minX = 0.0f;
	float maxX = 48.0f;
	float minZ = 0.0f;
	float maxZ = 48.0f;

	// 円形の移動範囲制限用
	float centerX = 24.0f;  // 円の中心X座標（デフォルト: マップの中央）
	float centerZ = 24.0f;  // 円の中心Z座標（デフォルト: マップの中央）
	float radius = 24.0f;   // 円の半径
};