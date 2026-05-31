#pragma once
#include <Scene/IScene.h>
#include "Compute.h"
#include <Camera/DebugCamera.h>
#include <Tool/Grid/Grid.h>
#include "Light/LightManager.h"
#include "Object/Gate.h"
#include <GameObject/Effect/Particle/MultiParticle/MultiParticle.h>
#include <Render/PostEffect.h>

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

};
