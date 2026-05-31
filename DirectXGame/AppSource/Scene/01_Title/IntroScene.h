#pragma once
#include <Scene/IScene.h>
#include "Compute.h"
#include <Camera/DebugCamera.h>
#include <Tool/Grid/Grid.h>

class IntroScene : public IScene {
public:

	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private:

	DebugCamera camera_;
	Grid grid_;

	std::unique_ptr<Compute> compute_;

};
