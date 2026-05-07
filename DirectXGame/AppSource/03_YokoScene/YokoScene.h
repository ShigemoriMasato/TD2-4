#pragma once
#include <Scene/IScene.h>
#include <Camera/Camera.h>
#include <Camera/DebugCamera.h>
#include <Render/RenderObject.h>
#include <Tool/Grid/Grid.h>

#include <GameObject/Effect/Trail/MultiTrail/MultiTrail.h>
#include <GameObject/Effect/Particle/MultiParticle/MultiParticle.h>
#include <GameObject/Effect/TrailOnParticle.h>

class YokoScene : public IScene
{
public:
	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private:
	// カメラ
	std::unique_ptr<DebugCamera> camera_;
	// ワールドgrid
	std::unique_ptr<Grid> grid_;

	// トレイル
	MultiTrail trail;
	int textureIndex_ = 0;

	// パーティクル
	MultiParticle particles_;

	// モデル
	std::unique_ptr<SHEngine::RenderObject> render_;
	Transform transform_;

	// パーティクル追従トレイル
	constexpr static int32_t kTrailOnParticle = 1;
	TrailOnParticle TrailOnParticle[kTrailOnParticle];


	bool start;
};