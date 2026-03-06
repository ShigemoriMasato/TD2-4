#include "Structures.h"
#include <algorithm>

Vector3 AABB::center()const
{
    return Vector3{
        (min.x + max.x) / 2.0f,
        (min.y + max.y) / 2.0f,
        (min.z + max.z) / 2.0f,
    };
}

void AABB::Move(const Vector3& move)
{
    this->min = this->min + move;
    this->max = this->max + move;
}

void AABB::Fix()
{
    AABB aabb = *this;
    this->min.x = std::min(aabb.min.x, aabb.max.x);
    this->max.x = std::max(aabb.min.x, aabb.max.x);
    this->min.y = std::min(aabb.min.y, aabb.max.y);
    this->max.y = std::max(aabb.min.y, aabb.max.y);
    this->min.z = std::min(aabb.min.z, aabb.max.z);
    this->max.z = std::max(aabb.min.z, aabb.max.z);
};

Vector3 AABB::GetCollisionDepth(const AABB& other)const
{
    Vector3 depth;
    depth.x = std::min(this->max.x, other.max.x) - std::max(this->min.x, other.min.x);
    depth.y = std::min(this->max.y, other.max.y) - std::max(this->min.y, other.min.y);
    depth.z = std::min(this->max.z, other.max.z) - std::max(this->min.z, other.min.z);
    if (depth.x <= 0.0f || depth.y <= 0.0f || depth.z <= 0.0f)
    {
        return { 0.0f, 0.0f, 0.0f };
    }
    return depth;
}

AABB AABB::Transform(const Matrix4x4& m) const
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
