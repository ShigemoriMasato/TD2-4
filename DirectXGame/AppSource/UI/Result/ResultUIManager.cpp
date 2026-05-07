#include "ResultUIManager.h"

using namespace SHEngine;

void ResultUIManager::Initialize(SHEngine::DrawData& drawData, int killCount, float clearTimer, KeyManager* keyManager) {
	scoreText_ = std::make_unique<ResultScoreText>();
	scoreText_->Initialize(drawData, killCount, keyManager);

	timerText_ = std::make_unique<ResultTimerText>();
	timerText_->Initialize(drawData, clearTimer, keyManager);
}

void ResultUIManager::Update(Matrix4x4 vpMatrix, float deltaTime) {
	scoreText_->Update(vpMatrix, deltaTime);
	timerText_->Update(vpMatrix, deltaTime);
}

void ResultUIManager::Draw(CmdObj* cmdObj) {
	scoreText_->Draw(cmdObj);
	timerText_->Draw(cmdObj);
}
