#pragma once
#include <Utility/Vector.h>
#include <Utility/Quaternion.h>

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

struct QuaternionTransform {
	Vector3 scale = { 1.0f, 1.0f, 1.0f };
	Quaternion rotate{};
	Vector3 position{};
};
