#pragma once
#include <Scene/IScene.h>

class CharaSelectScene : public IScene {
public:

	CharaSelectScene();

	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private:

};