#pragma once
#include <Scene/IScene.h>
#include <Camera/Camera.h>
#include <Camera/DebugCamera.h>

#include <Render/RenderObject.h>
#include <GameObject/Effect/Trail/TrailPresetRepository/TrailPresetRepository.h>
#include <GameObject/Effect/Trail/RibbonTrail/RibbonTrail.h>

class YokoScene : public IScene
{
public:
	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private:
	std::unique_ptr<DebugCamera> camera_;

	// Axe model
	int modelHandle_ = -1;
	NodeModelData modelData_{};
	int textureIndex_ = 0;
	std::unique_ptr<SHEngine::RenderObject> render_;
	Transform transform_{};
	Matrix4x4 wvp_{};

	// Trail preset cache & runtime
	TrailPresetRepository trailPresetRepo_;
	RibbonTrail ribbonTrail1_;
	RibbonTrail ribbonTrail2_;
};