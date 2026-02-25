#pragma once
#include <Scene/IScene.h>

class ItemManager;

class BackPackBuildScene : public IScene
{
public:
	BackPackBuildScene();
	~BackPackBuildScene();
	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private:

	std::unique_ptr<ItemManager> itemManager_;

};

