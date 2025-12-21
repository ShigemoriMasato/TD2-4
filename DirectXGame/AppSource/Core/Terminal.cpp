#include "Terminal.h"

void Terminal::Initialize() {
	engine_ = std::make_unique<SHEngine>();
	engine_->Initialize();

	sceneManager_ = std::make_unique<SceneManager>();
	sceneManager_->Initialize(engine_.get());
}

void Terminal::Run() {

	while (IsLoop()) {

		// 更新 ===
		engine_->Update();

		sceneManager_->Update();

		
		// 描画 ===
		engine_->PreDraw();

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
