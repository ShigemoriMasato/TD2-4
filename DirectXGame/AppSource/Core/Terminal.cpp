#include "Terminal.h"

void Terminal::Initialize() {
	engine_ = std::make_unique<SHEngine>();
	engine_->Initialize();

	sceneManager_ = std::make_unique<SceneManager>();
	sceneManager_->Initialize();
}

void Terminal::Run() {

	std::vector<std::unique_ptr<Window>> windows;
	windows.resize(50);
	for (int i = 0; i < 50; ++i) {
		windows[i] = engine_->MakeWindow(WindowConfig(), 0xffff00ff);
	}

	while (IsLoop()) {

		// 更新 ===
		engine_->Update();

		sceneManager_->Update();

		
		// 描画 ===
		engine_->PreDraw();

		for(const auto& window : windows) {
			window->PreDraw(true);
			window->PostDraw();
		}

		sceneManager_->Draw();


		engine_->EndFrame();

	}
}

bool Terminal::IsLoop() {
	bool engineMessage = engine_->IsLoop();

	//================================================
	//! アプリケーション側の条件を追加する
	//================================================

	return engineMessage;
}
