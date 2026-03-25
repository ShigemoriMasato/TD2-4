#include "IEnemy.h"
#include "EnemyManager.h"
#include <GameObject/Attack/IAttackObject.h>

void IEnemy::Initialize(Vector3* playerPos, EnemyManager* manager, int id) {
	drawInfo_.color = 0xff0000ff;
	playerPos_ = playerPos;
	manager_ = manager;
	id_ = id;
	collCircle_ = std::make_unique<Circle>();
	collCircle_->radius = 0.5f;
	CollConfig config;
	config.ownTag = CollTag::Enemy;
	config.targetTag = uint32_t(CollTag::Attack);
	config.colliderInfo = collCircle_.get();
	config.isActive = true;
	Collider::Initialize();
	SetColliderConfig(config);
	enemyHP_ = std::make_unique<EnemyHP>();
	enemyHP_->Initialize(modelManager_, drawDataManager_);
}

void IEnemy::Update(float deltaTime){
	enemyHP_->Update(deltaTime, static_cast<float>(hp_), static_cast<float>(maxHp_), position_);
}

void IEnemy::UpdateCollider() {
	drawInfo_.position = position_;
	collCircle_->center = { position_.x, position_.z };
	velocity_ = {}; // 毎フレームリセット
}

void IEnemy::OnCollision(Collider* other) {
	if (other->GetOwnTag() & CollTag::Enemy) {
		auto enemy = static_cast<IEnemy*>(other);
		Vector3 otherPos = enemy->GetPosition();
		Vector3 dir = (otherPos - drawInfo_.position).Normalize();
		float dist = (otherPos - drawInfo_.position).Length();
		//大体のdeltaTimeで押す
		position_ -= dir * dist * 0.016f * 8.0f;

		collCircle_->center = { position_.x, position_.z };
		return;
	}

	int id = other->GetID();
	auto it = damageIDs_.find(id);
	if (it != damageIDs_.end()) {
		// 既にダメージを受けている場合は無視
		return;
	}
	damageIDs_[id] = 1;
	auto attack = static_cast<IAttackObject*>(other);
	hp_ -= static_cast<int>(attack->GetDamage());

	if (hp_ <= 0) {
		KillMe();
	}
}

void IEnemy::KillMe() {
	if (hp_ <= 0.0f) {
		isActive_ = false;
	}
}

std::vector<DrawInfo> IEnemy::GetDrawInfos() const {
	auto info = enemyHP_->GetDrawInfo();
	// 敵の描画情報を追加
	info.push_back(drawInfo_);
	return info;
}

Vector2 IEnemy::WorldToScreenPos(const Vector3& worldPos, const Matrix4x4& viewProjectionMatrix, float screenWidth, float screenHeight) {
	// 1. ワールド座標 → クリップ空間 (手書き行列積)
	Vector4 clipSpacePos;
	clipSpacePos.x = worldPos.x * viewProjectionMatrix.m[0][0] + worldPos.y * viewProjectionMatrix.m[1][0] + worldPos.z * viewProjectionMatrix.m[2][0] + 1.0f * viewProjectionMatrix.m[3][0];

	clipSpacePos.y = worldPos.x * viewProjectionMatrix.m[0][1] + worldPos.y * viewProjectionMatrix.m[1][1] + worldPos.z * viewProjectionMatrix.m[2][1] + 1.0f * viewProjectionMatrix.m[3][1];

	clipSpacePos.z = worldPos.x * viewProjectionMatrix.m[0][2] + worldPos.y * viewProjectionMatrix.m[1][2] + worldPos.z * viewProjectionMatrix.m[2][2] + 1.0f * viewProjectionMatrix.m[3][2];

	clipSpacePos.w = worldPos.x * viewProjectionMatrix.m[0][3] + worldPos.y * viewProjectionMatrix.m[1][3] + worldPos.z * viewProjectionMatrix.m[2][3] + 1.0f * viewProjectionMatrix.m[3][3];

	// 2. w除算 → NDC
	if (clipSpacePos.w != 0.0f) {
		clipSpacePos.x /= clipSpacePos.w;
		clipSpacePos.y /= clipSpacePos.w;
	}

	// 3. NDC → スクリーン座標
	Vector2 screenPos;
	screenPos.x = (clipSpacePos.x + 1.0f) * 0.5f * screenWidth;
	screenPos.y = (1.0f - clipSpacePos.y) * 0.5f * screenHeight;

	return screenPos;
}
