#pragma once
#include"Camera/Camera.h"

class TitleCamera : public Camera {
public:

	void Initialize();

	void Update();

	void DebugDraw();
public:

	Matrix4x4 GetVpMatrix() const { return vpMatrix_; }

	// カメラのワールド座標位置を取得
	Vector3 GetCameraWorldPos() {
		// ワールド座標を入れる変数
		Vector3 worldPos;
		// ワールド行列の平行移動成分を取得
		worldPos.x = worldMatrix_.m[3][0];
		worldPos.y = worldMatrix_.m[3][1];
		worldPos.z = worldMatrix_.m[3][2];
		return worldPos;
	}

	bool IsAnimation() const { return isAnimation_; }

private:
	Matrix4x4 worldMatrix_;
	// カメラの目標座標
	Vector3 TargetCoordinate_ = {};

	float timer_ = 0.0f;
	float maxTime_ = 2.0f;

	bool isAnimation_ = true;

private:

	void MakeMatrix() override;
};