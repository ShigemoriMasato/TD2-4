#pragma once
#include <Scene/IScene.h>
#include "Compute.h"
#include <Camera/DebugCamera.h>
#include <Tool/Grid/Grid.h>
#include "Light/LightManager.h"
#include "Object/Gate.h"
#include <GameObject/Effect/Particle/MultiParticle/MultiParticle.h>
#include <Render/PostEffect.h>
#include <Render/RenderObject.h>
#include <Camera/Camera.h>

class IntroScene : public IScene {
public:

	~IntroScene();

	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private:

	void Save();
	void Load();

	DebugCamera camera_;
	Grid grid_;

	std::unique_ptr<Compute> compute_;

	LightManager lightManager_;
	Gate gate_;

	MultiParticle multiParticle1_;
	MultiParticle multiParticle2_;

	Transform particleTrans1_;
	Transform particleTrans2_;

	PostEffect postEffect_;
	PostEffectConfig postEffectConfig_;

	Fade fade_;

	bool fadeIn_ = false;

	// マウスカーソルスプライト
	std::unique_ptr<SHEngine::RenderObject> mouseCursorSprite_;
	int mouseCursorTexDefault_ = 0;
	int mouseCursorTexLeft_    = 0;
	int mouseCursorTexRight_   = 0;
	int mouseCursorTexBoth_    = 0;
	int mouseCursorTextureIndex_ = 0;
	Transform mouseCursorTransform_ = { {32.0f, 32.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
	std::unique_ptr<Camera> orthoCamera_;

};
