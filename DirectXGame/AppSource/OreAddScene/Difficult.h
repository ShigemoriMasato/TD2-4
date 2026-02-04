#pragma once
#include <Render/RenderObject.h>
#include <Utility/Easing.h>
#include <Assets/Model/ModelManager.h>
#include <UI/Object/FontObject.h>
#include <UI/Number.h>

class Difficult {
public:

	void Initialize(ModelManager* modelManager, DrawDataManager* drawDataManager, FontLoader* fontLoader, bool reset);
	void Update(float deltaTime);
	void Draw(Window* window, const Matrix4x4& vpMatrix);

	void DifficultyUp(bool up);
	bool AnimationEnd() { return timer_ >= duration_; }

private:

	float posY_ = 0.0f;
	float prevY_ = 0.0f;
	float targetY_ = 0.0f;
	Vector3 targetPosition_ = {};
	float targetRotateY_ = 0.0f;

	Transform modelParentTransform_ = {};
	Vector3 position_ = { 0.0f, 0.0f, 0.0f };
	Vector3 hovered_ = {};
	std::unique_ptr<RenderObject> triangle_ = nullptr;
	std::unique_ptr<RenderObject> meter_ = nullptr;
	int meterTextureIndex_ = -1;

	Matrix4x4 fontParentMat_ = Matrix4x4::Identity();
	std::vector<std::unique_ptr<FontObject>> orders_{};

	ModelManager* modelManager_ = nullptr;
	DrawDataManager* drawDataManager_ = nullptr;

	float t = 0.0f;
	float timer_ = 0.0f;
	const float duration_ = 4.0f;

	static inline int difficult_ = 0;

	float downHeight_ = 0.85f;

	bool uiDraw_ = false;

	std::unique_ptr<FontObject> difficultText_ = nullptr;
	std::unique_ptr<Number> difficultNumber_ = nullptr;

	int number = 0;
};
