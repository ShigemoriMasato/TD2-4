#pragma once
#include <Collision/Collider.h>
#include <GameObject/DrawInfo.h>
#include <Assets/Model/ModelManager.h>
#include "EnemyHP.h"

class EnemyManager;

class IEnemy : public Collider {
public:

	static void SetModelManager(SHEngine::ModelManager* modelManager) { modelManager_ = modelManager; }
	static void SetDrawDataManager(SHEngine::DrawDataManager* drawDataManager) { drawDataManager_ = drawDataManager; }

	virtual void Initialize(Vector3* playerPos, EnemyManager* manager, int id);
	void SetPosition(const Vector3& pos) { 
		drawInfo_.position = pos; 
		position_ = pos;
	}
	virtual void Update(float deltaTime);
	void UpdateCollider();

	Vector3 GetPosition() const { return position_; }
	DrawInfo GetDrawInfo() const { return drawInfo_; }
	void OnCollision(Collider* other) override;

	void KillMe();
	bool IsActive() const { return isActive_; }

	void SetHP(int hp) { hp_ = hp; maxHp_ = hp; }
	void SetVPMatrix(Matrix4x4 vpMatrix) { vpMatrix_ = vpMatrix; }
	void SetOrthoVPMatrix(Matrix4x4 vpMatrix) { orthoVpMatrix_ = vpMatrix; }

	void DrawUI(CmdObj* cmdObj);

protected:

	void SetModel(std::string path) { drawInfo_.modelIndex = modelManager_->LoadModel("Enemy/" + path); }

	static inline SHEngine::DrawDataManager* drawDataManager_ = nullptr;

	DrawInfo drawInfo_{};
	std::unique_ptr<Circle> collCircle_;
	Vector3* playerPos_;

	Vector3 velocity_{};
	Vector3 position_{};

	std::unique_ptr<EnemyHP> enemyHP_;

	Matrix4x4 vpMatrix_;
	Matrix4x4 orthoVpMatrix_;

private:

	Vector2 WorldToScreenPos(const Vector3& worldPos, const Matrix4x4& viewProjectionMatrix, float screenWidth, float screenHeight);

	static inline SHEngine::ModelManager* modelManager_ = nullptr;

	EnemyManager* manager_ = nullptr;
	int id_ = -1;
	bool isActive_ = true;
	int hp_ = 1;
	int maxHp_ = 1;

	std::map<int, int> damageIDs_;
};

