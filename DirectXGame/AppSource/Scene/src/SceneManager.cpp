#include "../SceneManager.h"
#include "Scene/InitializeScene.h"

SceneManager::~SceneManager() {
}

void SceneManager::Initialize(SHEngine* engine) {
	commonData_ = std::make_unique<CommonData>();
	nextScene_ = std::make_unique<InitializeScene>();
	engine_ = engine;
}

void SceneManager::Update() {
	SwapScene();

	// 現在のシーンの更新
	if (currentScene_) {
		nextScene_ = currentScene_->Update();
	}
}

void SceneManager::Draw() {
	currentScene_->Draw();
}



void SceneManager::SwapScene() {
	if (nextScene_) {
		currentScene_ = std::move(nextScene_);
		currentScene_->SwapScene(commonData_.get(), engine_);
		currentScene_->Initialize();
		nextScene_ = nullptr;
	}
}
