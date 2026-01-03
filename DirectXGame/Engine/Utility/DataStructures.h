#pragma once
#include <Utility/Vector.h>

struct VertexData {
	Vector4 position{};
	Vector2 texcoord{};
	Vector3 normal{};
};

struct Transform {
	Vector3 scale = { 1.0f, 1.0f, 1.0f };
	Vector3 rotate{};
	Vector3 position{};
};
