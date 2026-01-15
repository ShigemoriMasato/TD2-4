#pragma once
#include"Camera/Camera.h"
#include"Input/Input.h"

class CameraController {
public:

	void Initialize(Input* input);

	void Update();

	void DebugDraw();

public:

	Matrix4x4 GetVpMatrix() const { return camera_->GetVPMatrix(); }

private:

	Input* input_ = nullptr;

	// カメラ
	std::unique_ptr<Camera> camera_;
};