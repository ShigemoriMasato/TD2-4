#include "BatWeapon.h"
#include "../AppSource/GameObject/Enemy/EnemyManager.h"

using namespace SHEngine;

BatWeapon::BatWeapon(const WeaponData& data, SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager) : BaseWeapon(data) {
	int modelHandle = modelManager->LoadModel("Assets/Model/Weapon/Bat");
	auto modelData = modelManager->GetNodeModelData(modelHandle);
	auto drawData = drawDataManager->GetDrawData(modelData.drawDataIndex);

	auto& material = modelData.materials[modelData.materialIndex.front()];
	textureIndex_ = material.textureIndex;

	render_ = std::make_unique<RenderObject>();
	render_->Initialize();

	// シェーダー設定
	render_->psoConfig_.vs = "Game/Field.VS.hlsl";
	render_->psoConfig_.ps = "Game/Field.PS.hlsl";
	render_->SetUseTexture(true);

	// CBVの生成
	render_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER);
	render_->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "Color");
	render_->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");

	// 描画データを設定
	render_->SetDrawData(drawData);

	// ワールド座標を設定
	transform_.position = {0.0f, 0.0f, 0.0f};
	transform_.rotate = {0.0f, 0.0f, 0.0f};
	transform_.scale = {1.0f, 1.0f, 1.0f};

	// 単位行列の代入
	wvp_ = Matrix4x4::Identity();

	modelManager_ = modelManager;
	drawDataManager_ = drawDataManager;
}

void BatWeapon::Update(float deltaTime, WeaponController* controller, Matrix4x4 vpMatrix, Vector3 playerPos) {
	BaseWeapon::Update(deltaTime, controller, vpMatrix, playerPos);

	transform_.position = playerPos; // プレイヤーの位置に追従させる

	wvp_ = Matrix::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.position);
	wvp_ *= vpMatrix;
	render_->CopyBufferData(0, &wvp_, sizeof(Matrix4x4));

	Vector4 color = {1.0f, 1.0f, 1.0f, 1.0f};
	render_->CopyBufferData(1, &color, sizeof(Vector4));

	render_->CopyBufferData(2, &textureIndex_, sizeof(int));
}

void BatWeapon::Draw(CmdObj* cmdObj) { 
	render_->Draw(cmdObj); 
}

Vector3 BatWeapon::GetNearestEnemyDirection() {
	if (!enemyManager_) {
		return Vector3(0.0f, 0.0f, 1.0f); // デフォルト前方向
	}

	const auto& enemies = enemyManager_->GetEnemies();
	
	// 生きている敵がいない場合
	if (enemies.empty()) {
		return Vector3(0.0f, 0.0f, 1.0f);
	}

	// 最も近い敵を探す
	Enemy* nearestEnemy = nullptr;
	float minDistance = std::numeric_limits<float>::max();

	for (const auto& enemy : enemies) {
		if (!enemy || !enemy->IsAlive()) {
			continue;
		}

		Vector3 diff = enemy->GetPosition() - transform_.position;
		float distance = diff.Length();

		if (distance < minDistance) {
			minDistance = distance;
			nearestEnemy = enemy.get();
		}
	}

	// 最も近い敵への方向を返す
	if (nearestEnemy) {
		Vector3 diff = nearestEnemy->GetPosition() - transform_.position;
		return diff.Normalize();
	}

	return Vector3(0.0f, 0.0f, 1.0f);
}

void BatWeapon::Attack(WeaponController* controller) {
	// プレイヤーの位置と最も近い敵への方向を取得
	Vector3 spawnPos = transform_.position;
	Vector3 direction = GetNearestEnemyDirection();

	// 武器のパラメータを渡して斬撃オブジェクトの生成
	auto slash = std::make_unique<SlashAttack>(data_, spawnPos, direction, modelManager_, drawDataManager_);

	// コントローラーに攻撃オブジェクトを渡し、リストに追加してもらう
	controller->AddAttackObject(std::move(slash));
}