#include "../SceneManager.h"
#include "Scene/InitializeScene.h"

void SceneManager::Initialize() {
	commonData_ = std::make_unique<CommonData>();
	// 初期シーンの設定
	nextScene_ = std::make_unique<InitializeScene>();
}

void SceneManager::Update() {
	// シーン切り替えの処理
	if (nextScene_) {
		currentScene_ = std::move(nextScene_);
		currentScene_->Initialize();
		nextScene_ = nullptr;
	}

	// 現在のシーンの更新
	if (currentScene_) {
		nextScene_ = currentScene_->Update();
	}
}

void SceneManager::Draw() {

	currentScene_->Draw();

}
