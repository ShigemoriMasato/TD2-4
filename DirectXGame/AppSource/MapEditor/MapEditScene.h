#pragma once
#include <Scene/IScene.h>

class MapEditScene : public IScene {
public:

	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private:



};
