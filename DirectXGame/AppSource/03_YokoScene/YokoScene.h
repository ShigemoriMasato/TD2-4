#pragma once
#include <Scene/IScene.h>
#include <Camera/Camera.h>
#include <Camera/DebugCamera.h>

#include <GameObject/Effect/Trail/TestTrail1/TestTrail1.h>
#include <GameObject/Effect/Trail//TestTrail2/TestTrail2.h>
#include <GameObject/Effect/Trail//TestTrail3/TestTrail3.h>
#include <GameObject/Effect/Trail//TestTrail4/TestTrail4.h>

class YokoScene : public IScene
{
public:
	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private:
	std::unique_ptr<DebugCamera> camera_;

	std::unique_ptr<TestTrail1> testTrail1_;
	std::unique_ptr<TestTrail2> testTrail2_;
	std::unique_ptr<TestTrail3> testTrail3_;
	std::unique_ptr<TestTrail4> testTrail4_;

};