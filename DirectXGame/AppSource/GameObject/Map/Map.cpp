#include "Map.h"
#include <algorithm>

Map::Map() {}

void Map::Initialize(float minX, float maxX, float minZ, float maxZ) {
	minX_ = minX;
	maxX_ = maxX;
	minZ_ = minZ;
	maxZ_ = maxZ;
}

void Map::Update() {
	// 更新処理
}

bool Map::IsInBounds(const Vector3& position) const {
	return position.x >= minX_ && position.x <= maxX_ &&
		position.z >= minZ_ && position.z <= maxZ_;
}

Vector3 Map::ClampToBounds(const Vector3& position) const {
	Vector3 clamped = position;
	clamped.x = std::clamp(clamped.x, minX_, maxX_);
	clamped.z = std::clamp(clamped.z, minZ_, maxZ_);
	return clamped;
}
