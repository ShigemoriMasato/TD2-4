#pragma once
#include <SHEngine.h>
#include <Camera/Camera.h>
#include <Render/Renderer.h>

class Compute {
public:

	void Initialize(SHEngine::Engine* engine);
	void Update(float deltaTime, Camera* camera);
	void Draw(SHEngine::Screen::IDisplay* disp);

private:

	SHEngine::Engine* engine_ = nullptr;

	std::unique_ptr<CmdObj> compute_;
	std::unique_ptr<CmdObj> direct_;

};
