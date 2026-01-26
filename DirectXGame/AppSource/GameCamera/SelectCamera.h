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

private:

	Vector3 targetPos_ = {};
	Vector3 targetVelocity_ = {};

	// 座標補間割合
	float kInterpolationRate = 0.2f;
	// 速度掛け率
	float kVelocityBias = -0.5f;

	// カメラの目標座標
	Vector3 TargetCoordinate_ = {};

private:

	void FollowPos();

	void MakeMatrix() override;
};