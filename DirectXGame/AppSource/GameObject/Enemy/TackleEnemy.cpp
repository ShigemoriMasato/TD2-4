#include "TackleEnemy.h"
#include <cstdlib>
#include <ctime>

#ifdef USE_IMGUI
#include <imgui/imgui.h>
#endif

void TackleEnemy::Initialize(Vector3* playerPos, EnemyManager* manager, int id, Map* map) {
	IEnemy::Initialize(playerPos, manager, id, map);
	speed_ = float(rand() % 100) / 50.0f + baseApproachSpeed_;
	for (auto& drawInfo : drawInfo_) {
		drawInfo.scale = {1.f, 1.f, 1.f};
		drawInfo.color = 0xffff00ff; // Yellow color
	}
	SetModel("Tackle");
	state_ = State::Approach;
}

void TackleEnemy::Update(float deltaTime) {
	IEnemy::Update(deltaTime);

	Vector3 toPlayer = *playerPos_ - drawInfo_[0].position;
	float distance = toPlayer.Length();
	Vector3 direction = toPlayer.Normalize();

	switch (state_) {
	case State::Approach:
		position_ += direction * speed_ * deltaTime;
		if (distance < approachDistance_) {
			state_ = State::StepBack;
			stateTimer_ = stepBackDuration_;
		}
		break;
	case State::StepBack:
		position_ -= direction * baseStepBackSpeed_ * deltaTime;
		stateTimer_ -= deltaTime;
		if (stateTimer_ <= 0.0f) {
			state_ = State::Charge;
			stateTimer_ = chargeDuration_;
			chargeDirection_ = direction; // Lock the charge direction
		}
		break;
	case State::Charge:
		position_ += chargeDirection_ * baseChargeSpeed_ * deltaTime;
		stateTimer_ -= deltaTime;
		if (stateTimer_ <= 0.0f) {
			state_ = State::Approach;
			speed_ = float(rand() % 100) / 50.0f + baseApproachSpeed_; // Reset approach speed just in case
		}
		break;
	}

	ClampPositionToMap();
}

void TackleEnemy::DrawImGui() {
#ifdef USE_IMGUI
	ImGui::Begin("TackleEnemy ステータス");

	if (ImGui::TreeNode("速度設定")) {
		ImGui::DragFloat("接近速度", &baseApproachSpeed_, 0.1f, 0.1f, 10.0f);
		ImGui::DragFloat("後退速度", &baseStepBackSpeed_, 0.1f, 0.1f, 10.0f);
		ImGui::DragFloat("突撃速度", &baseChargeSpeed_, 0.1f, 0.1f, 20.0f);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("行動時間設定")) {
		ImGui::DragFloat("接近距離", &approachDistance_, 0.1f, 1.0f, 20.0f);
		ImGui::DragFloat("後退時間", &stepBackDuration_, 0.05f, 0.1f, 5.0f);
		ImGui::DragFloat("突撃時間", &chargeDuration_, 0.05f, 0.1f, 5.0f);
		ImGui::TreePop();
	}

	ImGui::End();
#endif
}
