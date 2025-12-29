#pragma once
#include <Scene/IScene.h>
#include <Render/RenderObject.h>

class TestScene : public IScene {
public:

	TestScene() = default;
	~TestScene() = default;

	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private:

	std::unique_ptr<RenderObject> renderObject_;

};
