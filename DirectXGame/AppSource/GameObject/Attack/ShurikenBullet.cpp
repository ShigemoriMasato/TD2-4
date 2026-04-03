#include "ShurikenBullet.h"
#include <random>

void ShurikenBullet::Initialize(const Config& config) {
	config_ = config;

	// 当たり判定の初期化
	collCircle_ = std::make_unique<Circle>();
	collCircle_->center = {config.position.x, config.position.z};
	collCircle_->radius = radius_ + 0.1f;
	CollConfig collConfig;
	collConfig.ownTag = CollTag::Attack;
	collConfig.targetTag = static_cast<uint32_t>(CollTag::Enemy);
	collConfig.colliderInfo = collCircle_.get();
	Collider::Initialize();
	SetCollider(collConfig);

	static std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> spreadDist(-config.spreadAngle / 2.0f, config.spreadAngle / 2.0f);
	float radian = config.direction + spreadDist(rng);

	direction_ = {cosf(radian), sinf(radian)};
	speed_ = 20.0f;

	// 跳ね返り回数として貫通力(penetration)を利用
	bounceCount_ = static_cast<int>(config.penetration);
	hitEnemyIds_.reserve(bounceCount_ + 1);
}

void ShurikenBullet::Update(float deltaTime) {
	collCircle_->center += direction_ * speed_ * deltaTime;

	rotate_ += 0.1f;

	timer_ += deltaTime;
	if (timer_ >= lifeTime_) {
		isActive_ = false;
	}
}

DrawInfo ShurikenBullet::GetDrawInfo() {
	DrawInfo info;
	info.position = {collCircle_->center.x, 0.0f, collCircle_->center.y};
	info.rotation.y = rotate_;
	info.scale = {radius_ * 2.0f, radius_ * 2.0f, radius_ * 2.0f};
	info.modelIndex = modelManager_->LoadModel("Item/Weapon/Shuriken");
	info.color = 0xffffffff;
	return info;
}

void ShurikenBullet::OnCollision(Collider* other) {
	if (!isActive_)
		return;

	int id = other->GetID();

	// すでに当たった敵には当たらないようにする
	for (int hitId : hitEnemyIds_) {
		if (hitId == id) {
			return;
		}
	}
	hitEnemyIds_.push_back(id);

	// 跳ね返り回数が残っている場合の処理
	if (bounceCount_ > 0 && enemyManager_) {
		bounceCount_--;

		auto enemies = enemyManager_->GetEnemies();
		IEnemy* nearestEnemy = nullptr;
		float minDistance = 999999.0f;
		Vector3 currentPos = {collCircle_->center.x, 0.0f, collCircle_->center.y};

		// 一番近い、まだ当たっていない敵を探す
		for (const auto& enemy : enemies) {
			int enemyId = enemy->GetID();

			// 既に当たった敵はスキップ
			bool alreadyHit = false;
			for (int hitId : hitEnemyIds_) {
				if (hitId == enemyId) {
					alreadyHit = true;
					break;
				}
			}
			if (alreadyHit)
				continue;

			Vector3 ePos = enemy->GetPosition();
			float dist = (ePos - currentPos).Length();

			if (dist < minDistance) {
				minDistance = dist;
				nearestEnemy = enemy;
			}
		}

		// 次のターゲットが見つかった場合、方向を変更する
		if (nearestEnemy) {
			Vector3 ePos = nearestEnemy->GetPosition();
			Vector3 dir = (ePos - currentPos).Normalize();
			direction_ = {dir.x, dir.z};

			// 跳ね返った際にタイマーをリセットし、次の敵まで届くようにする
			timer_ = 0.0f;
		} else {
			// 跳ね返る先（他の敵）がいない場合は消滅
			isActive_ = false;
		}
	} else {
		// 跳ね返り回数（攻撃回数）を使い切った場合は消滅
		isActive_ = false;
	}
}