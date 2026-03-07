#pragma once
#include <Scene/IScene.h>
#include <UI/Title/TitleUI.h>
#include <Camera/Camera.h>
#include <memory>

namespace Title {
	enum class State {
		Start,
		Options,
		Exit,
	};
}

class TitleScene : public IScene {
public:

	TitleScene();

	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private:

	Title::State state_ = Title::State::Start;
	std::unique_ptr<TitleUI> titleUI_ = nullptr;
	std::unique_ptr<Camera> camera_ = nullptr;
	Transform cameraTransform_{};
};