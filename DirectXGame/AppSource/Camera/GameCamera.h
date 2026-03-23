#pragma once
#include <Camera/Camera.h>

class GameCamera : public Camera {
public:

	void Initialize();
	void Update(float deltaTime, Vector3 position);
	void DrawImGui() override;

private:

	Vector3 offset_ = { -1.5f, 53.0f, 10.5f };
	bool viewToggle_ = false;
};
