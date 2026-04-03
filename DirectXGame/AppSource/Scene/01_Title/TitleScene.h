#pragma once
#include <Scene/IScene.h>
#include <UI/Title/TitleUI.h>
#include <Render/PostEffect.h>
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

	std::unique_ptr<PostEffect> postEffect_;
	PostEffectConfig postEffectConfig_;

};