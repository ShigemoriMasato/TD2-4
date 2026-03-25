#pragma once
#include <Camera/Camera.h>

class GameCamera : public Camera {
public:

	void Initialize();
	void Update(float deltaTime, Vector3 position);
	void DrawImGui() override;

private:

	Vector3 offset_ = { 2.0f, 34.0f, -45.0f };
	bool isViewType2 = false;

};