#include "../SceneManager.h"
#include "Scene/InitializeScene.h"
#include <Common/DebugParam/GameParamEditor.h>
#include <LightManager.h>

SceneManager::~SceneManager() {
	auto& scores = commonData_->maxGoldNum_;
	if (!scores.empty()) {
		int allGold = 0;
		BinaryManager binaryManager;
		for (const auto& score : scores) {
			allGold += score;
			binaryManager.RegisterOutput(score);
		}
		int safeNumber = allGold / (int)scores.size();
		binaryManager.RegisterOutput(safeNumber);
		binaryManager.Write("Scores");
	}

	LightManager::GetInstance()->Finalize();
	GameParamEditor::GetInstance()->Finalize();
}

void SceneManager::Initialize(SHEngine* engine) {
	commonData_ = std::make_unique<CommonData>();
	// 初期シーンの設定
	nextScene_ = std::make_unique<InitializeScene>();
	engine_ = engine;

	BinaryManager binaryManager;
	auto values = binaryManager.Read("Scores");
	if (!values.empty()) {
		size_t index = 0;
		int allScore = 0;
		while (index < values.size() - 1) {
			int score = BinaryManager::Reverse<int>(values[index++].get());
			commonData_->maxGoldNum_.push_back(score);
			allScore += score;
		}
		int safeNumber = BinaryManager::Reverse<int>(values[index++].get());
		if (safeNumber != allScore / commonData_->maxGoldNum_.size()) {
			getLogger("Game")->error("Score data corrupted. Resetting scores.");
			commonData_->maxGoldNum_.clear();
		}
	}
}

void SceneManager::Update() {
	SwapScene();

	// 現在のシーンの更新
	if (currentScene_) {
		nextScene_ = currentScene_->Update();
	}
}

void SceneManager::Draw() {
	//絶対よくない
	LightManager::GetInstance()->Update();

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
