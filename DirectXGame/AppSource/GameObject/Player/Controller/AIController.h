#pragma once
#include "GameObject/Enemy/EnemyManager.h"
#include "GameObject/Player/PlayerAI/PlayerAI.h"
#include "IController.h"

class AIController : public IController {
public:
	AIController(Vector3* playerPos, EnemyManager* enemyManager):playerPos_(playerPos),enemyManager_(enemyManager){
		ai_=std::make_unique<PlayerAI>();
	}

	// 移動方向を返す
	Vector2 GetMoveDirection() override;

	// ダッシュが入力されたか
	bool IsDashTriggered() override;

private:
	Vector3* playerPos_ = nullptr;
	EnemyManager* enemyManager_ = nullptr;
	std::unique_ptr<PlayerAI> ai_ = nullptr;
};
