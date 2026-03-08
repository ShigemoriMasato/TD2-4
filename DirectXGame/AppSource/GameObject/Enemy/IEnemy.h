#pragma once
#include <Collision/Collider.h>
#include <GameObject/DrawInfo.h>

class EnemyManager;

class IEnemy  : public Collider {
public:

	virtual void Initialize(Vector3* playerPos, EnemyManager* manager, int id);
	void SetPosition(const Vector3& pos) { drawInfo_.position = pos; }
	virtual void Update(float deltaTime) {};

	DrawInfo GetDrawInfo() const { return drawInfo_; }
	void OnCollision(Collider* other) override;

	void KillMe();
	bool IsActive() const { return isActive_; }

protected:

	DrawInfo drawInfo_{};
	std::unique_ptr<Circle> collCircle_;
	Vector3* playerPos_;

private:

	EnemyManager* manager_ = nullptr;
	int id_ = -1;
	bool isActive_ = true;
	int hp_ = 1;

};

