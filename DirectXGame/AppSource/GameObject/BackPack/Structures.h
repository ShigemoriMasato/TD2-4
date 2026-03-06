#pragma once
#include <cstdint>
#include "Utility/Vector.h"

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

struct Vector2intHash
{
    size_t operator()(const Vector2int& v) const noexcept
    {
        uint64_t x = static_cast<uint64_t>(v.x);
        uint64_t y = static_cast<uint64_t>(v.y);
        return (x * 73856093) ^ (y * 19349663);
    }
};

struct Vector3int
{
    int x = 0;
    int y = 0;
    int z = 0;
    Vector3int operator+(const Vector3int& rhs) const
    {
        return Vector3int{ x + rhs.x, y + rhs.y, z + rhs.z };
    }
    Vector3int operator-(const Vector3int& rhs) const
    {
        return Vector3int{ x - rhs.x, y - rhs.y, z - rhs.z };
    }
    bool operator==(const Vector3int& rhs) const
    {
        return x == rhs.x && y == rhs.y && z == rhs.z;
    }
    bool operator!=(const Vector3int& rhs) const
    {
        return x != rhs.x || y != rhs.y || z != rhs.z;
    }
};

struct Vector4int
{
    int x = 0, y = 0, z = 0, w = 0;
    Vector4int operator+(const Vector4int& rhs) const
    {
        return Vector4int{ x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w };
    }
    Vector4int operator-(const Vector4int& rhs) const
    {
        return Vector4int{ x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w };
    }
    bool operator==(const Vector4int& rhs) const
    {
        return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
    }
    bool operator!=(const Vector4int& rhs) const
    {
        return x != rhs.x || y != rhs.y || z != rhs.z || w != rhs.w;
    }
};

struct AABB
{
    Vector3 min;
    Vector3 max;

    // 中心座標
    Vector3 center()const;
    // 移動　※自身が変化する※
    void Move(const Vector3& move);
    // min,maxが入れ替わる可能性があれば毎フレーム飛び出したい
    void Fix();
    // 衝突している時は深度ベクトルを返す. 衝突していない時は(0,0,0)を返す
    Vector3 GetCollisionDepth(const AABB& other)const;
    // ローカル AABB → ワールド AABB
    AABB Transform(const Matrix4x4& m) const;

    AABB operator+(const Vector3& rhs) const
    {
        return AABB{ min + rhs, max + rhs };
    }

};

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
