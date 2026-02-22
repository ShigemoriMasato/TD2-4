#pragma once
#include <Scene/IScene.h>

class GameScene : public IScene {
public:

	GameScene();

	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private:

};