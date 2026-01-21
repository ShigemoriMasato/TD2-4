#include "ColliderManager.h"
#include <Collision/Data/CollisionVisitor.h>

void ColliderManager::CollisionCheckAll() {
	for (const auto& collider : colliders_) {
		CollisionCheck(collider);
	}
}

void ColliderManager::CollisionCheck(Collider* collider) {
	for (const auto& other : colliders_) {
		//互いに当たってしかるべきタグか確認
		if (!(collider->targetTag_ & other->ownTag_ && other->targetTag_ & collider->ownTag_)) {
			continue;
		}

		//自分自身とは当たらない
		if (other == collider) {
			continue;
		}
		 
		//当たり判定処理
		bool isHit = std::visit(CollisionVisitor(), collider->colliderInfo_, other->colliderInfo_);
		if (isHit) {
			collider->OnCollision(other);
			other->OnCollision(collider);
		}
	}
}

void ColliderManager::AddCollider(Collider* collider) {
	colliders_.emplace_back(collider);
}

void ColliderManager::DeleteCollider(Collider* collider) {
	for (size_t i = 0; i < colliders_.size(); ++i) {
		if (colliders_[i] == collider) {
			colliders_.erase(colliders_.begin() + i);
			return;
		}
	}
}
