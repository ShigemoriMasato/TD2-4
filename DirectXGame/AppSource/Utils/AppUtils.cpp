#include "AppUtils.h"

Vector3 AppUtils::WorldToScreenPos(const Vector3& worldPos, const Matrix4x4& viewProjectionMatrix, float screenWidth, float screenHeight) {
	// 1. ワールド → クリップ空間
	Vector4 clipSpacePos;
	clipSpacePos.x = worldPos.x * viewProjectionMatrix.m[0][0] + worldPos.y * viewProjectionMatrix.m[1][0] + worldPos.z * viewProjectionMatrix.m[2][0] + 1.0f * viewProjectionMatrix.m[3][0];

	clipSpacePos.y = worldPos.x * viewProjectionMatrix.m[0][1] + worldPos.y * viewProjectionMatrix.m[1][1] + worldPos.z * viewProjectionMatrix.m[2][1] + 1.0f * viewProjectionMatrix.m[3][1];

	clipSpacePos.z = worldPos.x * viewProjectionMatrix.m[0][2] + worldPos.y * viewProjectionMatrix.m[1][2] + worldPos.z * viewProjectionMatrix.m[2][2] + 1.0f * viewProjectionMatrix.m[3][2];

	clipSpacePos.w = worldPos.x * viewProjectionMatrix.m[0][3] + worldPos.y * viewProjectionMatrix.m[1][3] + worldPos.z * viewProjectionMatrix.m[2][3] + 1.0f * viewProjectionMatrix.m[3][3];

	// 2. w除算 → NDC
	clipSpacePos.x /= clipSpacePos.w;
	clipSpacePos.y /= clipSpacePos.w;

	// 3. NDC → スクリーン座標
	Vector3 screenPos;
	screenPos.x = (clipSpacePos.x + 1.0f) * 0.5f * screenWidth;
	screenPos.y = (1.0f - clipSpacePos.y) * 0.5f * screenHeight;
	screenPos.z = 0.0f;

	return screenPos;
}