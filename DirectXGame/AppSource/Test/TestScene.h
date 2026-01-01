#pragma once
#include <Scene/IScene.h>
#include <Render/RenderObject.h>
#include <Camera/DebugCamera.h>

class TestScene : public IScene {
public:

	TestScene() = default;
	~TestScene() = default;

	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private:

	std::unique_ptr<DebugCamera> debugCamera_;

	struct VSData {
		Matrix4x4 worldMatrix = Matrix4x4::Identity();
		Matrix4x4 vpMatrix = Matrix4x4::Identity();
	};
	std::unique_ptr<RenderObject> renderObject_;
	VSData vsData_{};
	int vsDataIndex_ = -1;

};
