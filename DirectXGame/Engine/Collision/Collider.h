#pragma once
#include <variant>
#include "Data/CollisionInfo.h"

class ColliderManager;

enum class CollTag : uint32_t {
	None,
	Player = 1 << 0,
	Enemy = 1 << 1,
	Item = 1 << 2,
	Stage = 1 << 3
};

uint32_t operator|(CollTag a, CollTag b);
uint32_t operator|(uint32_t a, CollTag b);
uint32_t operator&(uint32_t a, CollTag b);
uint32_t operator~(CollTag a);

struct CollConfig {
	CollTag ownTag = CollTag::None;
	uint32_t targetTag = 0;
	bool isActive = false;
};

class Collider {
public:

	Collider() = default;
	virtual ~Collider() = default;

	void Initialize();
	void SetColliderConfig(const CollConfig& circle);

	void SetActivate(bool isActive = true);

	virtual void OnCollision(Collider* other) = 0;

private:

	static ColliderManager* colliderManager_;

private:

	friend class ColliderManager;

	std::variant<Circle, Quad> colliderInfo_;
	int id_ = -1;

	bool isActive_ = true;
};
