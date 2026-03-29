#pragma once
#include <Camera/Camera.h>

// Forward declaration
namespace SHEngine {
	class Input;
}

class GameCamera : public Camera {
public:

	void Initialize();
	void SetInput(SHEngine::Input* input) { input_ = input; }
	void Update(float deltaTime, Vector3 position);
	void DrawImGui() override;

private:

	Vector3 offset_ = { -6.5f, 30.0f, -45.0f };
	bool isViewType2 = false;
	
	SHEngine::Input* input_ = nullptr;
	float distanceScale_ = 1.0f;
	const float minDistanceScale_ = 0.5f;
	const float maxDistanceScale_ = 2.0f;
	const float wheelSensitivity_ = 0.00075f;

};