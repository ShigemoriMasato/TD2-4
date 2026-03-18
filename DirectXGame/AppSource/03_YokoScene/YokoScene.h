#pragma once
#include <Scene/IScene.h>
#include <Camera/Camera.h>
#include <Camera/DebugCamera.h>

#include <Render/RenderObject.h>
#include <GameObject/Effect/Trail/TrailPresetDataBank/TrailPresetDataBank.h>
#include <GameObject/Effect/Trail/MultiTrail/MultiTrail.h>

class YokoScene : public IScene
{
public:
	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private:
	std::unique_ptr<DebugCamera> camera_;

	int textureIndex_ = 0;
	std::unique_ptr<SHEngine::RenderObject> render_;
	Transform transform_{};
	Matrix4x4 wvp_{};

	TrailPresetDataBank trailPresetRepo_;

	MultiTrail trails_;
};