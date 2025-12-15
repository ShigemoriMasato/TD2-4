#pragma once
#include <Scene/SceneManager.h>
#include <SHEngine.h>

class Terminal {
public:
	
	Terminal() = default;
	~Terminal() = default;

	void Initialize();

	void Run();

private:

	bool IsLoop();

	std::unique_ptr<SHEngine> engine_;
	std::unique_ptr<SceneManager> sceneManager_;

	std::unique_ptr<Window> mainWindow_;

};
