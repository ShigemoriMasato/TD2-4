#include "IAttackObject.h"

void IAttackObject::SetCollider(CollConfig& config) {
	config.ownTag = CollTag::Attack;
	config.targetTag = static_cast<uint32_t>(CollTag::Enemy);
	config.isActive = true;
	Collider::Initialize();
	SetColliderConfig(config);
}
