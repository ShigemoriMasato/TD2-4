#pragma once
#include "CollisionInfo.h"

class CollisionVisitor {
public:
	bool operator()(Circle* colliderA, Circle* colliderB);
	bool operator()(Circle* colliderA, Quad* colliderB);
	bool operator()(Quad* colliderA, Circle* colliderB);
	bool operator()(Quad* colliderA, Quad* colliderB);

	bool IsHit() const { return isHit_; }

private:
	bool isHit_ = false;
};
