#pragma once
#include <Scene/IScene.h>
#include <Camera/Camera.h>
#include <Camera/DebugCamera.h>

#include <Render/RenderObject.h>
#include <GameObject/Effect/Trail/DataBank/TrailPresetDataBank.h>
#include <GameObject/Effect/Trail/MultiTrail/MultiTrail.h>

class YokoScene : public IScene
{
public:
	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private:
	std::unique_ptr<DebugCamera> camera_;

	TrailPresetDataBank trailDataBank_;

	/// Axe
	MultiTrail trail_Axe;
	int textureIndex_ = 0;
	std::unique_ptr<SHEngine::RenderObject> render_;
	Transform axeTransform_;

	/// ええ感じ１
	MultiTrail trail_test1;
	//Transform transform_{};
};