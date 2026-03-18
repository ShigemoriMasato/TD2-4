#pragma once
#include <Scene/IScene.h>
#include <Camera/Camera.h>
#include <Camera/DebugCamera.h>

#include <Render/RenderObject.h>
#include <GameObject/Effect/Trail/TrailPresetRepository/TrailPresetRepository.h>
#include <GameObject/Effect/Trail/RibbonTrail2Point/RibbonTrail2Point.h>

class YokoScene : public IScene
{
public:
	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private:
	std::unique_ptr<DebugCamera> camera_;

	// Axe model
	int axeModelHandle_ = -1;
	NodeModelData axeModelData_{};
	int axeTextureIndex_ = 0;
	std::unique_ptr<SHEngine::RenderObject> axeRender_;
	Transform axeTransform_{};
	Matrix4x4 axeWvp_{};

	// Trail preset cache & runtime
	TrailPresetRepository trailPresetRepo_;
	RibbonTrail2Point ribbonTrail_;
};