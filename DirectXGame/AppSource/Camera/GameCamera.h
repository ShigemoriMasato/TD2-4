#pragma once
#include <Camera/Camera.h>

class GameCamera : public Camera {
public:

	void Initialize();
	void Update(float deltaTime, Vector3 position);
	void DrawImGui() override;

private:

	Vector3 offset_ = { 0.0f, 13.7f, -35.0f };

};
