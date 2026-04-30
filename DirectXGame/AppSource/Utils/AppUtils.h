#pragma once
#include <SHEngine.h>

class AppUtils {
public:
	static Vector3 WorldToScreenPos(const Vector3& worldPos, const Matrix4x4& viewProjectionMatrix, float screenWidth, float screenHeight);
};
