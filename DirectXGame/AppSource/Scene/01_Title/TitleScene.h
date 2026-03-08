#pragma once
#include <Scene/IScene.h>
#include <UI/Title/TitleUI.h>
#include <Camera/Camera.h>
#include <memory>

class TitleScene : public IScene {
public:

	TitleScene();

	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private:

	std::unique_ptr<TitleUI> titleUI_;
	std::unique_ptr<Camera> camera_;

};