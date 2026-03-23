#pragma once
#include <Collision/Collider.h>
#include <GameObject/DrawInfo.h>
#include <Assets/Model/ModelManager.h>

class EnemyManager;

class IEnemy : public Collider {
public:

	static void SetModelManager(SHEngine::ModelManager* modelManager) { modelManager_ = modelManager; }
	static SHEngine::ModelManager* GetModelManager() { return modelManager_; }

	virtual void Initialize(Vector3* playerPos, EnemyManager* manager, int id);
	void SetPosition(const Vector3& pos) { 
		drawInfo_.position = pos; 
		position_ = pos;
	}
	virtual void Update(float deltaTime) {};
	void UpdateCollider();

	Vector3 GetPosition() const { return position_; }
	DrawInfo GetDrawInfo() const { return drawInfo_; }
	void OnCollision(Collider* other) override;

	void KillMe();
	bool IsActive() const { return isActive_; }

	void SetHP(float hp) { hp_ = hp; }
	float GetAttack() const { return attack_; }

protected:

	void SetModel(std::string path) { drawInfo_.modelIndex = modelManager_->LoadModel("Enemy/" + path); }
	void SetAttack(float attack) { attack_ = attack; }

	DrawInfo drawInfo_{};
	std::unique_ptr<Circle> collCircle_;
	Vector3* playerPos_;

	Vector3 velocity_{};
	Vector3 position_{};

private:

	static inline SHEngine::ModelManager* modelManager_ = nullptr;

	EnemyManager* manager_ = nullptr;
	int id_ = -1;
	bool isActive_ = true;
	float hp_ = 1.0f;
	float attack_ = 1.0f;

	std::map<int, int> damageIDs_;
};

