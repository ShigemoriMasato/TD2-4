#include "IEnemy.h"
#include "EnemyManager.h"
#include <GameObject/Attack/IAttackObject.h>

void IEnemy::Initialize(Vector3* playerPos, EnemyManager* manager, int id) {
	drawInfo_.color = 0xff0000ff;
	playerPos_ = playerPos;
	manager_ = manager;
	id_ = id;
	collCircle_ = std::make_unique<Circle>();
	collCircle_->radius = 0.5f;
	CollConfig config;
	config.ownTag = CollTag::Enemy;
	config.targetTag = uint32_t(CollTag::Attack);
	config.colliderInfo = collCircle_.get();
	config.isActive = true;
	Collider::Initialize();
	SetColliderConfig(config);
}

void IEnemy::OnCollision(Collider* other) {
	auto attack = static_cast<IAttackObject*>(other);
	hp_ -= static_cast<int>(attack->GetDamage());
	KillMe();
}

void IEnemy::KillMe() {
	isActive_ = false;
}
