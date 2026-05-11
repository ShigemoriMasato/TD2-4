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


	void SetOffset(const Vector3& offset) { offset_ = offset; }
	void Setrotation(const Vector3& rotation) { rotation_ = rotation; }
private:

	Vector3 offset_ = { 0.0f, 35.0f, -35.0f };
	bool isViewType2 = false;
	bool manualControl_ = false;

	SHEngine::Input* input_ = nullptr;
	float distanceScale_ = 1.0f;
	const float minDistanceScale_ = 0.5f;
	const float maxDistanceScale_ = 1.0f;
	const float wheelSensitivity_ = 0.00075f;

};