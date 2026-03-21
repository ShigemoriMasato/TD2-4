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

void IEnemy::UpdateCollider() {
	drawInfo_.position = position_;
	collCircle_->center = { position_.x, position_.z };
	velocity_ = {}; // 毎フレームリセット
}

void IEnemy::OnCollision(Collider* other) {
	if (other->GetOwnTag() & CollTag::Enemy) {
		auto enemy = static_cast<IEnemy*>(other);
		Vector3 otherPos = enemy->GetDrawInfo().position;
		Vector3 dir = (otherPos - drawInfo_.position).Normalize();
		float dist = (otherPos - drawInfo_.position).Length();
		//大体のdeltaTimeで押す
		position_ -= dir * dist * 0.016f * 8.0f;

		collCircle_->center = { position_.x, position_.z };
		return;
	}

	int id = other->GetID();
	auto it = damageIDs_.find(id);
	if (it != damageIDs_.end()) {
		// 既にダメージを受けている場合は無視
		return;
	}
	damageIDs_[id] = 1;
	auto attack = static_cast<IAttackObject*>(other);
	hp_ -= static_cast<float>(attack->GetDamage());

	if (hp_ <= 0.0f) {
		KillMe();
	}
}

void IEnemy::KillMe() {
	if (hp_ <= 0.0f) {
		isActive_ = false;
	}
}
