#pragma once
#include <Utility/Vector.h>
#include <Utility/MatrixFactory.h>
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
	Matrix4x4 GetMatrix() const {
		Matrix4x4 S = Matrix::MakeScaleMatrix(scale);
		Matrix4x4 R = Matrix::MakeRotationMatrix(rotate);
		Matrix4x4 T = Matrix::MakeTranslationMatrix(position);
		return S * R * T;
	}
};

struct QuaternionTransform {
	Vector3 scale = { 1.0f, 1.0f, 1.0f };
	Quaternion rotate{};
	Vector3 position{};
};

struct DirectionalLight {
	Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
	Vector3 direction = { 0.0f, -1.0f, 0.0f };
	float intensity = 1.0f;
};

struct PointLight {
	Vector4 color;
	Vector3 position;
	float intensity;
	float radius;
	float decay;
	Vector2 padding;
};
