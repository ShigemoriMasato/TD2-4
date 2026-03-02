#include "Enemy.h"
#include <Render/RenderObject.h>
#include <Assets/Model/ModelManager.h>
#include <Utility/MyMath.h>
#include <Utility/MatrixFactory.h>

Enemy::Enemy() = default;

Enemy::~Enemy() = default;

void Enemy::Initialize() { 

}

void Enemy::Update(float deltaTime) {
	if (!isAlive_) {
		return;
	}

	// 生成後の待機時間処理
	if (!canMove_) {
		spawnTimer_ += deltaTime;
		if (spawnTimer_ >= spawnWaitTime_) {
			canMove_ = true;
		}
		return;
	}

	// 移動処理
	Move(deltaTime);

	// HPが0以下なら死亡
	if (param_.hp <= 0) {
		isAlive_ = false;
	}
}

void Enemy::Draw() {
	if (!isAlive_) {
		return;
	}
}

void Enemy::Setup(EnemyType type, const Vector3& position, const Vector3& targetPosition) {
	type_ = type;
	position_ = position;
	targetPosition_ = targetPosition;
	param_ = GetEnemyParam(type);
	isAlive_ = true;
	canMove_ = false;
	spawnTimer_ = 0.0f;
	state_ = EnemyState::None;

	// ターゲットへの方向ベクトルを計算
	Vector3 toTarget = targetPosition_ - position_;
	float length = MyMath::Length(toTarget);
	if (length > 0.0f) {
		direction_ = MyMath::Normalize(toTarget);
	} else {
		direction_ = { 0.0f, 0.0f, 1.0f };
	}
}

void Enemy::TakeDamage(int damage) {
	if (!isAlive_) {
		return;
	}

	param_.hp -= damage;
	if (param_.hp < 0) {
		param_.hp = 0;
	}
}

EnemyParam Enemy::GetEnemyParam(EnemyType type) {
	EnemyParam param{};

	switch (type) {
	case EnemyType::Normal:
		param.hp = 100;
		param.attack = 10;
		param.speed = 2;
		break;
	case EnemyType::Speed:
		param.hp = 50;
		param.attack = 5;
		param.speed = 5;
		break;
	default:
		param.hp = 100;
		param.attack = 10;
		param.speed = 2;
		break;
	}

	return param;
}

void Enemy::Move(float deltaTime) {
	// ターゲットに向かって移動
	Vector3 velocity = direction_ * static_cast<float>(param_.speed) * deltaTime;
	position_ = position_ + velocity;

	// ターゲットに到達したかチェック（距離が一定以下なら到達）
	Vector3 toTarget = targetPosition_ - position_;
	float distanceToTarget = MyMath::Length(toTarget);
	if (distanceToTarget < 0.5f) {
		// ターゲットに到達したので死亡扱いにする（またはプレイヤーにダメージを与える処理を追加）
		isAlive_ = false;
	}
}
