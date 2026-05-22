#pragma once
#include <Scene/IScene.h>
#include "WeaponImGui.h"
#include "WeaponRender.h"

class WeaponEditScene : public IScene {
public:

	WeaponEditScene();
	~WeaponEditScene();

	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private:

	std::unique_ptr<WeaponImGui> weaponImGui_;

	std::unique_ptr<Weapon> weapon_;
	std::unique_ptr<WeaponRender> weaponRender_;

	int currentRenderID_ = -1;

	bool isStop_ = false;

};
