#pragma once
#include <Scene/IScene.h>
#include <Render/RenderObject.h>
#include <Camera/DebugCamera.h>
#include <Render/DebugLine.h>

#include"Common/DebugParam/ParamManager.h"

class TestScene : public IScene {
public:

	TestScene() = default;
	~TestScene() = default;

	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private:

	std::unique_ptr<DebugCamera> debugCamera_;
};
