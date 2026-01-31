#pragma once
#include <Scene/IScene.h>

class OreAddScene : public IScene {
public:

	OreAddScene() = default;
	~OreAddScene() override = default;

	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private:

	int fontTexture_ = -1;

};
