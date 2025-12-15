#include "Terminal.h"

void Terminal::Initialize() {
	engine_ = std::make_unique<SHEngine>();
	engine_->Initialize();

	sceneManager_ = std::make_unique<SceneManager>();
	sceneManager_->Initialize();
}

void Terminal::Run() {
	while (engine_->IsLoop()) {

		// 更新 ===
		engine_->Update();

		sceneManager_->Update();

		
		
		// 描画 ===
		engine_->PreDraw();


		sceneManager_->Draw();


		engine_->EndFrame();

	}
}
