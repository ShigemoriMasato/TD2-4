#pragma once
#include <SHEngine.h>
#include <Render/RenderObject.h>
#include <Utility/Vector.h>
#include <Utility/Matrix.h>
#include <Utility/DataStructures.h>
#include <Utility/MatrixFactory.h>

// XZ平面に平行な四角形の構造体
struct PlaneXZ
{
	Vector3 center; // 平面の中心
	float width = 1.0f; // 平面の幅
	float height = 1.0f; // 平面の高さ

	PlaneXZ Translate(Vector3 translation) const
	{
		PlaneXZ translatedPlane;
		translatedPlane.center = this->center + translation;
		translatedPlane.width = this->width;
		translatedPlane.height = this->height;
		return translatedPlane;
	}
};

struct AABB
{
	Vector3 min;
	Vector3 max;


	// ローカル AABB → ワールド AABB
	AABB Transform(const Matrix4x4& m) const
	{
		Vector3 corners[8] = {
			{ min.x, min.y, min.z },
			{ max.x, min.y, min.z },
			{ min.x, max.y, min.z },
			{ max.x, max.y, min.z },
			{ min.x, min.y, max.z },
			{ max.x, min.y, max.z },
			{ min.x, max.y, max.z },
			{ max.x, max.y, max.z }
		};

		AABB transformedAABB;
		transformedAABB.min = { FLT_MAX,  FLT_MAX,  FLT_MAX };
		transformedAABB.max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

		for (int i = 0; i < 8; ++i)
		{
			Vector3 p = m * corners[i];

			transformedAABB.min.x = std::min(transformedAABB.min.x, p.x);
			transformedAABB.min.y = std::min(transformedAABB.min.y, p.y);
			transformedAABB.min.z = std::min(transformedAABB.min.z, p.z);

			transformedAABB.max.x = std::max(transformedAABB.max.x, p.x);
			transformedAABB.max.y = std::max(transformedAABB.max.y, p.y);
			transformedAABB.max.z = std::max(transformedAABB.max.z, p.z);
		}

		return transformedAABB;
	}
};



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

struct InstanceBinding
{
	int matricesSrvIndex = -1; // VS t0
	int vpCbvIndex = -1;       // VS b0
	int texCbvIndex = -1;      // PS b0
	int colorCbvIndex = -1;    // PS b1
	int lightCbvIndex = -1;    // PS b2
};

struct Vector2int
{
	int x = 0;
	int y = 0;
	Vector2int operator+(const Vector2int& rhs) const
	{
		return Vector2int{ x + rhs.x, y + rhs.y };
	}
	Vector2int operator-(const Vector2int& rhs) const
	{
		return Vector2int{ x - rhs.x, y - rhs.y };
	}
	bool operator==(const Vector2int& rhs) const
	{
		return x == rhs.x && y == rhs.y;
	}
	bool operator!=(const Vector2int& rhs) const
	{
		return x != rhs.x || y != rhs.y;
	}
	bool operator<(const Vector2int& rhs) const
	{
		if (y != rhs.y)
		{
			return y < rhs.y;
		}
		return x < rhs.x;
	}
};
