#pragma once
#include "GameObject/EasingAnimation/EasingAnimation.h"
#include <Render/RenderObject.h>
#include <SHEngine.h>
#include <assets/Model/ModelManager.h>

class LetterBox {
public:
	void Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager);
	void Update(Matrix4x4 vpMatrix, float deltaTime);
	void Draw(CmdObj* cmdObj);

	void Trigger();
	bool GetIsTriggered() const { return isTriggered_; }
	bool GetIsActive() const { return easing_.GetIsActive(); }

private:
	std::unique_ptr<SHEngine::RenderObject> render_;
	Transform transformTop_;
	Transform transformBottom_;
	EasingAnimation<float> easing_;

	float progress_ = 0.0f;
	bool isTriggered_ = false;

	float screenWidth_ = 1280.0f;
	float screenHeight_ = 720.0f;
	float boxHeight_ = 100.0f;
};