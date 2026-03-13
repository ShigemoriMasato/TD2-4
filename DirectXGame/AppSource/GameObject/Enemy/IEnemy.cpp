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
	config.targetTag = CollTag::Attack | CollTag::Enemy;
	config.colliderInfo = collCircle_.get();
	config.isActive = true;
	Collider::Initialize();
	SetColliderConfig(config);
}

void IEnemy::OnCollision(Collider* other) {
	if(other->GetOwnTag() & CollTag::Enemy) {
		auto enemy = static_cast<IEnemy*>(other);
		Vector3 otherPos = enemy->GetDrawInfo().position;
		Vector3 dir = (otherPos - drawInfo_.position).Normalize();
		float dist = (otherPos - drawInfo_.position).Length();
		drawInfo_.position += dir * dist;
		if ((otherPos - drawInfo_.position).Length() < 1.0f) {
			getLogger("Enemy")->warn("Not Resolved !?!?!?!?!?!?!?!?");
		} else {
			getLogger("Enemy")->warn("Problem Clear !");
		}
		collCircle_->center = { drawInfo_.position.x, drawInfo_.position.z };
		return;
	}

	auto attack = static_cast<IAttackObject*>(other);
	hp_ -= static_cast<int>(attack->GetDamage());
	KillMe();
}

void IEnemy::KillMe() {
	isActive_ = false;
}
