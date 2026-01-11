#include "ColliderManager.h"

void ColliderManager::CollisionCheckAll() {
}

void ColliderManager::AddCollider(Collider* collider) {
	colliders_.emplace_back(collider);
}
