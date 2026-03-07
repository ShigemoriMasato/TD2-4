#pragma once
#include <Scene/IScene.h>

class ResultScene : public IScene {
public:

	ResultScene();

	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private:

};