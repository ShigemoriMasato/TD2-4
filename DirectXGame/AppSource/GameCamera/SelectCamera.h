#pragma once
#include"Camera/Camera.h"

class SelectCamera : public Camera {
public:

	void Initialize();

	void Update();

	void DebugDraw();
public:

	Matrix4x4 GetVpMatrix() const { return vpMatrix_; }

	void SetTargetPos(const Vector3& target, const Vector3& velocity) { 
		targetPos_ = target;
		targetVelocity_ = velocity;
	}

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

private:

	Vector3 targetPos_ = {};
	Vector3 targetVelocity_ = {};

	// 座標補間割合
	float kInterpolationRate = 0.2f;
	// 速度掛け率
	float kVelocityBias = -0.5f;

	Matrix4x4 worldMatrix_;

	// カメラの目標座標
	Vector3 TargetCoordinate_ = {};

private:

	void FollowPos();

	void MakeMatrix() override;
};