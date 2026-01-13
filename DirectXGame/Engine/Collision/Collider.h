#pragma once
#include <variant>
#include "Data/CollisionInfo.h"
#include <Tool/Logger/Logger.h>

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
	std::variant<Circle*, Quad*> colliderInfo;
	bool isActive = false;
};

class Collider {
public:

	Collider() = default;
	virtual ~Collider();

	static void SetColliderManager(ColliderManager* manager) { colliderManager_ = manager; }

	//当たり判定時の処理
	virtual void OnCollision(Collider* other) = 0;

protected:

	//ColliderManagerへの登録
	void Initialize();
	void SetColliderConfig(const CollConfig& config);
	void SetActive(bool isActive = true) { isActive_ = isActive; };

private:

	static ColliderManager* colliderManager_;
	static Logger logger_;
	
private:

	friend class ColliderManager;

	std::variant<Circle*, Quad*> colliderInfo_;
	CollTag ownTag_ = CollTag::None;
	uint32_t targetTag_ = 0;
	int id_ = -1;

	bool isActive_ = true;
};
