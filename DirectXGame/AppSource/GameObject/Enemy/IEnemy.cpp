#include "IEnemy.h"
#include "EnemyManager.h"

void IEnemy::Initialize(Vector3* playerPos, EnemyManager* manager, int id) {
	drawInfo_.color = 0xff0000ff;
	playerPos_ = playerPos;
	manager_ = manager;
	id_ = id;
	collCircle_ = std::make_unique<Circle>();
	CollConfig config;
	config.ownTag = CollTag::Enemy;
	config.targetTag = uint32_t(CollTag::Player);
	config.colliderInfo = collCircle_.get();
	config.isActive = true;
	Collider::Initialize();
	SetColliderConfig(config);
}

void IEnemy::OnCollision(Collider* other) {
	KillMe();
}

void IEnemy::KillMe() {
	isActive_ = false;
}
