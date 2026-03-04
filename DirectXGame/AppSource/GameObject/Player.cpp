#include "Player.h"
#include <imgui/imgui.h>

void Player::Initialize(KeyManager* keyManager) {
	keyManager_ = keyManager;

	CollConfig config;
	config.ownTag = CollTag::Player;
	config.targetTag = CollTag::Enemy | CollTag::Item;
	collCircle_ = std::make_unique<Circle>();
	config.colliderInfo = collCircle_.get();
	config.isActive = true;
	Collider::Initialize();
	SetColliderConfig(config);

	collCircle_->radius = 0.5f;
	collCircle_->center = { position_.x, position_.z };
}

void Player::Update(float deltaTime) {
	auto keyStates = keyManager_->GetKeyStates();
	if (keyStates[Key::Up]) {
		position_.z += moveSpeed_ * deltaTime;
	}
	if (keyStates[Key::Down]) {
		position_.z -= moveSpeed_ * deltaTime;
	}
	if (keyStates[Key::Left]) {
		position_.x -= moveSpeed_ * deltaTime;
	}
	if (keyStates[Key::Right]) {
		position_.x += moveSpeed_ * deltaTime;
	}

	collCircle_->center = { position_.x, position_.z };

	drawInfo_.position = position_;
	drawInfo_.scale = { 1.0f, 1.0f, 1.0f };
	drawInfo_.timer += deltaTime;

	collCircle_->center = { position_.x, position_.z };
}

void Player::DrawImGui() {
#ifdef USE_IMGUI

	ImGui::Begin("Player Info");
	ImGui::DragFloat("Speed", &moveSpeed_, 0.1f, 0.0f);
	ImGui::End();

#endif
}

void Player::OnCollision(Collider* other) {

}
