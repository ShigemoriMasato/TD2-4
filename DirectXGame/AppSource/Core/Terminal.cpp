#include "Terminal.h"

void Terminal::Initialize(HINSTANCE hInstance) {
	engine_ = std::make_unique<SHEngine>();
	engine_->Initialize(hInstance);

	sceneManager_ = std::make_unique<SceneManager>();
	sceneManager_->Initialize(engine_.get());
}

void Terminal::Run() {

	while (IsLoop()) {

		// 更新 ===
		engine_->Update();

		if (sceneManager_->IsHasNextScene()) {
			engine_->WaitForGPU();
			sceneManager_->SwapScene();
		}

		sceneManager_->Update();

		
		// 描画 ===
		if (engine_->PreDraw()) {

			sceneManager_->Draw();

			engine_->EndFrame();
		}

	}
}

bool Terminal::IsLoop() {
	if (sceneManager_->IsExeFinished()) {
		return engine_->IsLoop();
	}

	return false;
}
