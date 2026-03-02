#include "ScreenRaycaster.h"
#include <Utility/MatrixFactory.h>
#include <cassert>

ScreenRaycaster::ScreenRaycaster(float screenWidth, float screenHeight) : screenWidth_(screenWidth), screenHeight_(screenHeight) {}

void ScreenRaycaster::SetCamera(const Matrix4x4& view, const Matrix4x4& proj) {
	view_ = view;                                // ビュー行列
	proj_ = proj;                                // 射影行列
	invVP_ = Matrix4x4(view_ * proj_).Inverse(); // ビュープロジェクション逆行列
}

Vector2 ScreenRaycaster::ScreenToNDC(float screenX, float screenY) {
	float ndcX = (2.0f * screenX) / screenWidth_ - 1.0f;
	float ndcY = 1.0f - (2.0f * screenY) / screenHeight_;

	return Vector2(ndcX, ndcY); // カメラ座標を返す
}

Vector3 ScreenRaycaster::ScreenToWorldGround(float screenX, float screenY) {
	Ray ray = ScreenToRay(screenX, screenY);

	// 平面との交点
	if (std::abs(ray.direction.y) > 0.0001f) {
		float t = (0.0f - ray.origin.y) / ray.direction.y;
		return ray.origin + ray.direction * t;
	}

	// 水平レイの場合はNearを返す
	return ray.origin;
}

Ray ScreenRaycaster::ScreenToRay(float screenX, float screenY) {

	// 左下が０、右上が１とした時のマウスポジション
	float ndcX = (screenX / screenWidth_) * 2.0f - 1.0f;
	float ndcY = 1.0f - (screenY / screenHeight_) * 2.0f; // Yは上下反転

	// クリップ空間でZ=0(near)とZ=1(far)の2点を作る
	Vector4 nearPoint = { ndcX, ndcY, 0.0f, 1.0f };
	Vector4 farPoint = { ndcX, ndcY, 1.0f, 1.0f };

	// 逆射影行列
	Matrix4x4 inverseViewProj = invVP_;

	// ワールド空間に変換
	Vector4 nearWorld = Transform(nearPoint, inverseViewProj);
	Vector4 farWorld = Transform(farPoint, inverseViewProj);

	// マウスレイの始点・方向
	Ray ray;
	ray.origin = { nearWorld.x / nearWorld.w, nearWorld.y / nearWorld.w, nearWorld.z / nearWorld.w };
	ray.direction = Vector3{
	(farWorld.x / farWorld.w) - ray.origin.x,
	(farWorld.y / farWorld.w) - ray.origin.y,
	(farWorld.z / farWorld.w) - ray.origin.z
	}.Normalize();

	return ray;
}

Vector4 ScreenRaycaster::Transform(Vector4& vector, Matrix4x4& matrix) {
	Vector4 result = {};
	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + vector.w * matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + vector.w * matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + vector.w * matrix.m[3][2];
	result.w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + vector.w * matrix.m[3][3];
	return result;
}
