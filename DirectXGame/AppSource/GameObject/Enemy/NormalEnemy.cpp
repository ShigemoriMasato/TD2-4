#include "NormalEnemy.h"

void NormalEnemy::Initialize(Vector3* playerPos, EnemyManager* manager, int id, Map* map) {
	IEnemy::Initialize(playerPos, manager, id, map);
	speed_ = float(rand() % 100) / 50.0f + baseSpeed_;
	for (auto& drawInfo : drawInfo_) {
		drawInfo.scale = scale_;
	}

	localPositions_[0] = {0.0f, 0.0f, 0.0f}; // Body
	localPositions_[1] = {0.0f, 0.0f, 0.0f}; // Hand_L
	localPositions_[2] = {0.0f, 0.0f, 0.0f}; // Hand_R
	localPositions_[3] = {0.0f, 0.0f, 0.0f}; // Leg

	SetModel("Body", 0);
	SetModel("Hand_L", 1);
	SetModel("Hand_R", 2);
	SetModel("Leg", 3);
}

void NormalEnemy::Update(float deltaTime) {
	IEnemy::Update(deltaTime);
	Vector3 direction = (*playerPos_ - drawInfo_[0].position).Normalize();
	position_ += direction * speed_ * deltaTime;
	ClampPositionToMap();

	// 待機時アニメーション
	IdleAnimation(deltaTime);
}

void NormalEnemy::IdleAnimation(float deltaTime) {
	switch (state_) {
	case AnimState::Forward: {
		// Forwardが動いていなければ開始
		if (!posAnimForward_.anim.GetIsActive()) {
			posAnimForward_.anim.Start({0.0f, 0.0f, 0.0f}, {0.0f, 0.5f, 0.0f}, 1.0f, EaseType::EaseInOutBack);
		}

		bool playing = posAnimForward_.anim.Update(deltaTime, posAnimForward_.temp);

		if (playing) {
			localPositions_[0] = posAnimForward_.temp;
			localPositions_[1] = posAnimForward_.temp;
			localPositions_[2] = posAnimForward_.temp;
		} else {
			state_ = AnimState::Backward;
		}
		break;
	}
	case AnimState::Backward: {
		if (!posAnimBackward_.anim.GetIsActive()) {
			posAnimBackward_.anim.Start({0.0f, 0.5f, 0.0f}, {0.0f, 0.0f, 0.0f}, 1.0f, EaseType::EaseInOutExpo);
		}

		bool playing = posAnimBackward_.anim.Update(deltaTime, posAnimBackward_.temp);

		if (playing) {
			localPositions_[0] = posAnimBackward_.temp;
			localPositions_[1] = posAnimBackward_.temp;
			localPositions_[2] = posAnimBackward_.temp;
		} else {
			state_ = AnimState::Forward;
		}
		break;
	}
	}
}