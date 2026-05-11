#pragma once
#include <Scene/IScene.h>
#include <Render/Font/Text.h>
#include <Render/PostEffect.h>
#include "GameObject/EasingAnimation/AnimationBundle.h"
#include "GameObject/Result/ResultSword.h"
#include "UI/Result/ResultUIManager.h"

class ResultScene : public IScene {
public:

	ResultScene();

	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private:

	void UpdateSelectVisual();

private:
	std::unique_ptr<SHEngine::Text> clearText_;
	std::unique_ptr<SHEngine::Text> gameOverText_;
	std::unique_ptr<SHEngine::Text> retryText_;
	std::unique_ptr<SHEngine::Text> toTitleText_;
	std::unique_ptr<ResultUIManager> uiManager_;

	std::unique_ptr<ResultSword> sword_;

	std::unique_ptr<Camera> camera_;
	std::unique_ptr<Camera> orthoCamera_;

	Transform clearTextTransform_{};
	Transform gameOverTextTransform_{};
	Transform retryTextTransform_{};
	Transform toTitleTextTransform_{};

	bool isWin_ = false;

	std::unique_ptr<PostEffect> postEffect_;
	PostEffectConfig postEffectConfig_;

	AnimationBundle<float> posAnime_;
	float alphaTime_ = 0.0f;
	float posYTime_ = 0.0f;
	float pendingTime_ = 0.0f;

	int selectedIndex_ = 0;
	AnimationBundle<float> decideScaleAnime_;
	bool isDeciding_ = false;

	// カメラシェイク用
	bool isCameraShaking_ = false;
	float shakeTime_ = 0.0f;
	float shakeDuration_ = 0.5f;
	float shakeIntensity_ = 0.5f;
	Vector3 cameraBasePos_;
	Vector3 orthoCameraBasePos_;

	bool isPreFinished_ = false;
};