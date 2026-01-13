#pragma once
#include "Collider.h"
#include <vector>

class ColliderManager {
public:

	void CollisionCheckAll();

	void CollisionCheck(Collider* collider);

	void AddCollider(Collider* collider);
	void DeleteCollider(Collider* collider);

private:

	std::vector<Collider*> colliders_;

};
