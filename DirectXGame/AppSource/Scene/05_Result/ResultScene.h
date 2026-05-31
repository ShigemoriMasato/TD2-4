#pragma once
#include <Scene/IScene.h>
#include <Render/Font/Text.h>
#include <Render/PostEffect.h>
#include "GameObject/EasingAnimation/AnimationBundle.h"
#include "GameObject/Result/ResultSword.h"
#include "UI/Result/ResultUIManager.h"
#include "GameObject/Fade/FadeManager.h"
#include "GameObject/Result/EnemyRainManager.h"
#include <GameObject/Effect/Particle/MultiParticle/MultiParticle.h>
#include <UI/Game/GameFrame.h>

class ResultScene : public IScene {
public:

	ResultScene();
	~ResultScene();

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
	std::unique_ptr<FadeManager> fadeManager_;

	std::unique_ptr<ResultSword> sword_;
	std::unique_ptr<EnemyRainManager> enemyRainManager_;

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
	
	DirectionalLight dirLight_;
	Bloom bloom_;

	std::unique_ptr<GameFrame> gameFrame_;

	// 火花Particle
	std::unique_ptr<MultiParticle> sparkParticle_;
	Transform sparkParticleTransform_;
	Matrix4x4 sparkParticleModelWorld_;
	bool mrSecond = false;

	Vector2 lastMousePos_ = {0.0f, 0.0f};
	Vector2 hitBoxSize_ = {200.0f, 60.0f};

	float retryTextMarginX_ = 120.0f;
	float toTitleTextMarginX_ = 150.0f;

	int lastSelectedIndex_ = -1;

	std::unique_ptr<PlayData> bgm_ = nullptr;

	// マウスカーソルスプライト
	std::unique_ptr<SHEngine::RenderObject> mouseCursorSprite_ = nullptr;
	int mouseCursorTextureIndex_ = -1;
	int mouseCursorTexDefault_ = -1;
	int mouseCursorTexLeft_ = -1;
	int mouseCursorTexRight_ = -1;
	int mouseCursorTexBoth_ = -1;
	Transform mouseCursorTransform_ = { {32.0f, 32.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };

};
