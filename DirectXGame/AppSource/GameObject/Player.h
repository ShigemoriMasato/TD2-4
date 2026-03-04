#pragma once
#include <Common/KeyConfig/KeyManager.h>
#include <Render/RenderObject.h>
#include <Collision/Collider.h>
#include "DrawInfo.h"

class Player : public Collider {
public:

	void Initialize(KeyManager* keyManager);
	void Update(float deltaTime);
	void DrawImGui();
	DrawInfo GetDrawInfo() const { return drawInfo_; }
	Vector3* GetPositionPtr() { return &position_; }

	void OnCollision(Collider* other) override;

private:

	KeyManager* keyManager_ = nullptr;
	Vector3 position_{};
	float moveSpeed_ = 5.0f;
	std::unique_ptr<Circle> collCircle_;
	DrawInfo drawInfo_{};

};
