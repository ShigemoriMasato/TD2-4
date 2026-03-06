#pragma once
#include <Scene/IScene.h>

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
};