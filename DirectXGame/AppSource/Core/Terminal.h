#pragma once
#include <SHEngine.h>
#include <Scene/SceneManager.h>

class Terminal {
public:

	Terminal(HINSTANCE hInstance);
	
	void Run();

private:

	bool IsLoop() const;

	SHEngine::D3DResourceLeakChecker leakChecker_;

	std::unique_ptr<SHEngine::Engine> engine_ = nullptr;
	std::unique_ptr<SceneManager> sceneManager_ = nullptr;

};
