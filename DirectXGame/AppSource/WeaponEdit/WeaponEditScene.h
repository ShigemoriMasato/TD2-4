#pragma once
#include <Scene/IScene.h>
#include "WeaponImGui.h"

class WeaponEditScene : public IScene {
public:

	WeaponEditScene();
	~WeaponEditScene();

	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private:

	std::unique_ptr<WeaponImGui> weaponImGui_;

};
