#pragma once
#include <Scene/IScene.h>
#include <Camera/Camera.h>
#include <Camera/DebugCamera.h>
#include <Render/RenderObject.h>

#include <GameObject/Effect/Trail/DataBank/TrailPresetDataBank.h>
#include <GameObject/Effect/Trail/MultiTrail/MultiTrail.h>
#include <GameObject/Effect/Particle/DataBank/ParticlePresetDataBank.h>
#include <GameObject/Effect/Particle/MultiParticle/MultiParticle.h>

class YokoScene : public IScene
{
public:
	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private:
	std::unique_ptr<DebugCamera> camera_;

	TrailPresetDataBank trailDataBank_;
	ParticlePresetDataBank particleDataBank_;

	/// Axeトレイル
	MultiTrail trail;
	int textureIndex_ = 0;
	std::unique_ptr<SHEngine::RenderObject> render_;
	Transform transform_;

	bool start;
};