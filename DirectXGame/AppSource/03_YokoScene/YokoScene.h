#pragma once
#include <Scene/IScene.h>
#include <Camera/Camera.h>
#include <Camera/DebugCamera.h>
#include <Render/RenderObject.h>

#include <GameObject/Effect/Trail/MultiTrail/MultiTrail.h>

#include <GameObject/Effect/Particle/DataBank/ParticlePresetDataBank.h>
#include <GameObject/Effect/Particle/MultiParticle/MultiParticle.h>
#include <GameObject/Effect/sparkEffect.h>
#include <GameObject/Effect/Particle/Drawer/ParticleDrawer.h>

class YokoScene : public IScene
{
public:
	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private:
	std::unique_ptr<DebugCamera> camera_;

	ParticlePresetDataBank particleDataBank_;


	// トレイル
	MultiTrail trail;
	int textureIndex_ = 0;

	// モデル
	std::unique_ptr<SHEngine::RenderObject> render_;
	Transform transform_;

	// パーティクル
	SparkEffect sparkEffect;


	bool start;
};