#pragma once
#include <SHEngine.h>
#include <Render/RenderObject.h>
#include <Utility/Vector.h>
#include <Utility/Matrix.h>
#include <Utility/DataStructures.h>
#include <Utility/MatrixFactory.h>
#include <Tool/Json/JsonManager.h>


namespace GameConstants
{
	// バックパックの行数
	inline constexpr size_t kBackPackRowNum = 8;
	// バックパックの列数
	inline constexpr size_t kBackPackColNum = 12;

	// バックパックの1マスのサイズ
	inline constexpr float kBackPackCellSize = 1.0f;
	// バックパックのセル間サイズ
	inline constexpr float kBackPackCellSpacing = 1.0f;
}

enum class GridState
{
	// ロック中・解放不可
	LockedUnavailable,
	// ロック中・解放可能
	LockedAvailable,
	// 解放済み・空
	UnlockedEmpty,
	// 解放済み・アイテムあり
	UnlockedOccupied,
};

struct InstanceBinding
{
	int matricesSrvIndex = -1; // VS t0
	int vpCbvIndex = -1;       // VS b0
	int texCbvIndex = -1;      // PS b0
	int colorCbvIndex = -1;    // PS b1
	int lightCbvIndex = -1;    // PS b2
};