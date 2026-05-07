#pragma once
#include "ResultScoreText.h"
#include "ResultTimerText.h"

/// <summary>
/// リザルトシーンのUI管理クラス
/// </summary>
class ResultUIManager {
public:
	void Initialize(SHEngine::DrawData& drawData, int killCount, float clearTimer, KeyManager* keyManager);
	void Update(Matrix4x4 vpMatrix, float deltaTime);
	void Draw(CmdObj* cmdObj);

private:
	std::unique_ptr<ResultScoreText> scoreText_;
	std::unique_ptr<ResultTimerText> timerText_;
};
