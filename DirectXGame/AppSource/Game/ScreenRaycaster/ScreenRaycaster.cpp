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
	// スクリーン座標からカメラ座標に変換
	Vector2 ndc = ScreenToNDC(screenX, screenY);

	// NDC上のNearとFarの点を作成
	Vector4 nearPos = {ndc.x, ndc.y, 0.0f, 1.0f};
	Vector4 farPos = {ndc.x, ndc.y, 1.0f, 1.0f};

	// ビュープロジェクション逆行列でカメラNDCからワールド空間へ変換
	Vector4 worldNear4 = Transform(nearPos, invVP_);
	Vector4 worldFar4 = Transform(farPos, invVP_);

	// w除算
	Vector3 vNear = {
	    worldNear4.x / worldNear4.w,
	    worldNear4.y / worldNear4.w,
	    worldNear4.z / worldNear4.w,
	};

	Vector3 vFar = {
	    worldFar4.x / worldFar4.w,
	    worldFar4.y / worldFar4.w,
	    worldFar4.z / worldFar4.w,
	};

	// レイの生成
	Ray ray;
	ray.origin = vNear;
	ray.direction = Vector3(vFar - vNear).Normalize();

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
