#include "Enemy.h"
#include <Render/RenderObject.h>
#include <Assets/Model/ModelManager.h>
#include <Utility/MyMath.h>
#include <Utility/MatrixFactory.h>
#include <Utility/Matrix.h>
#include <Utility/Color.h>

int Enemy::nextID_ = 0;

Enemy::Enemy() : id_(nextID_++) {}

Enemy::~Enemy() = default;

void Enemy::Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager) {
	if (!modelManager || !drawDataManager) {
		return;
	}

	// RenderObjectが既に存在する場合は再初期化しない
	if (render_) {
		return;
	}

	render_ = std::make_unique<SHEngine::RenderObject>();
	render_->Initialize();
	render_->psoConfig_.vs = "Simple.VS.hlsl";
	render_->psoConfig_.ps = "Color.PS.hlsl";

	render_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER);
	colorIndex_ = render_->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER);

	int modelHandle = modelManager->LoadModel("Assets/.EngineResource/Model/Cube");

	auto modelData = modelManager->GetNodeModelData(modelHandle);
	auto drawData = drawDataManager->GetDrawData(modelData.drawDataIndex);
	render_->SetDrawData(drawData);

	wvp_ = Matrix4x4::Identity();

	// Transformの初期化
	transform_.scale = { 2.0f, 2.0f, 2.0f };
	transform_.rotate = { 0.0f, 0.0f, 0.0f };
	transform_.position = { 0.0f, 0.0f, 0.0f };
}

void Enemy::Update(float deltaTime, const Vector3& playerPosition, const Matrix4x4& vpMatrix) {
	if (!render_) {
		return;
	}

	// 生存していない場合は更新しない
	if (!isAlive_) {
		return;
	}

	// ターゲット位置を常にプレイヤーの位置に更新
	targetPosition_ = playerPosition;

	// ターゲットへの方向ベクトルを再計算
	Vector3 toTarget = targetPosition_ - position_;
	float length = MyMath::Length(toTarget);
	if (length > 0.0f) {
		direction_ = MyMath::Normalize(toTarget);
	}

	// 生成後の待機時間処理
	if (!canMove_) {
		spawnTimer_ += deltaTime;
		if (spawnTimer_ >= spawnWaitTime_) {
			canMove_ = true;
		}
	} else {
		// 移動処理
		Move(deltaTime);
	}

	// HPが0以下なら死亡
	if (param_.hp <= 0) {
		isAlive_ = false;
		return;
	}

	// positionをtransformに反映
	transform_.position = position_;

	// 色の更新（待機中は赤、移動可能時は白）
	Vector4 color = canMove_ ? Vector4(1.0f, 1.0f, 1.0f, 1.0f) : Vector4(1.0f, 0.0f, 0.0f, 1.0f);
	render_->CopyBufferData(colorIndex_, &color, sizeof(Vector4));

	// WVP行列の計算
	wvp_ = Matrix::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.position);
	wvp_ *= vpMatrix;

	// wvp行列を描画に適用
	render_->CopyBufferData(0, &wvp_, sizeof(Matrix4x4));
}

void Enemy::Draw(CmdObj* cmdObj) {
	if (!isAlive_ || !render_) {
		return;
	}

	render_->Draw(cmdObj);
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

	//// ターゲットに到達したかチェック（距離が一定以下なら到達）
	//Vector3 toTarget = targetPosition_ - position_;
	//float distanceToTarget = MyMath::Length(toTarget);
	//if (distanceToTarget < 0.5f) {
	//	// ターゲットに到達したので死亡扱いにする
	//	isAlive_ = false;
	//}
}
