#pragma once
#include <Scene/IScene.h>
#include <Title/WaterWave/WaterWave.h>
#include <Title/Title/Title.h>

class TitleScene : public IScene {
public:

	TitleScene();
	~TitleScene();

	void Initialize() override;
	std::unique_ptr<IScene> Update() override;
	void Draw() override;

private:

	std::unique_ptr<Camera> camera_ = nullptr;

	std::unique_ptr<Title> title_ = nullptr;
	std::unique_ptr<WaterWave> waterWave_ = nullptr;
	std::unique_ptr<RenderObject> gameMonitor_ = nullptr;

};
