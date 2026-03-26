#pragma once
#include <Camera/Camera.h>

class GameCamera : public Camera {
public:

	void Initialize();
	void Update(float deltaTime, Vector3 position);
	void DrawImGui() override;

private:

	Vector3 offset_ = { -6.5f, 27.0f, -40.0f };
	bool isViewType2 = false;

};