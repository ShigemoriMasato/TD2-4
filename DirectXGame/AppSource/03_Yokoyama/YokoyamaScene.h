#pragma once
#include <Scene/IScene.h>

class ItemManager;

class YokoyamaScene : public IScene
{
public:

	YokoyamaScene();
	~YokoyamaScene();

	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private:

	std::unique_ptr<ItemManager> itemManager_;

};
