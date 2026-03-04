#include "Projectile.h"
#include "../AppSource/GameObject/Enemy/Enemy.h"
#include <Utility/Matrix.h>
#include <Utility/MatrixFactory.h>
#include <Utility/MyMath.h>

using namespace SHEngine;

Projectile::Projectile(const DamageInfo& damageInfo, const Vector3& position, const Vector3& direction, float speed)
	: speed_(speed) {
	damageInfo_ = damageInfo;
	transform_.position = position;
	transform_.scale = { 0.3f, 0.3f, 0.3f };
	direction_ = MyMath::Normalize(direction);
	penetrationCount_ = static_cast<int>(damageInfo_.knockbackPower);
	wvp_ = Matrix4x4::Identity();
}

Projectile::~Projectile() = default;

void Projectile::Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager) {
	// 描画オブジェクトの初期化
	render_ = std::make_unique<SHEngine::RenderObject>();
	render_->Initialize();

	// シェーダーの設定
	render_->psoConfig_.vs = "Game/Field.VS.hlsl";
	render_->psoConfig_.ps = "Game/Field.PS.hlsl";
	render_->SetUseTexture(true);

	// モデルの読み込み
	int modelHandle = modelManager->LoadModel("Assets/.EngineResource/Model/cube");
	auto modelData = modelManager->GetNodeModelData(modelHandle);
	auto drawData = drawDataManager->GetDrawData(modelData.drawDataIndex);
	render_->SetDrawData(drawData);

	// CBVの設定
	render_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER);
	render_->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "Color");

	// WVP行列を初期化
	wvp_ = Matrix4x4::Identity();
}

void Projectile::Update(float deltaTime) {
	if (!isActive_) {
		return;
	}

	// ライフタイムを減少
	lifetime_ -= deltaTime;
	if (lifetime_ <= 0.0f) {
		isActive_ = false;
		return;
	}

	// 移動処理
	Vector3 velocity = direction_ * speed_ * deltaTime;
	transform_.position = transform_.position + velocity;

	// WVP行列を計算
	wvp_ = Matrix::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.position);
}

void Projectile::CheckCollisionWithEnemies(const std::vector<std::unique_ptr<Enemy>>& enemies) {
	if (!isActive_) {
		return;
	}

	for (const auto& enemy : enemies) {
		if (!enemy || !enemy->IsAlive()) {
			continue;
		}

		// 敵との距離を計算
		Vector3 toEnemy = enemy->GetPosition() - transform_.position;
		float distance = toEnemy.Length();

		// 当たり判定チェック
		if (distance < collisionRadius_ + 0.5f) {
			// すでに当たった敵はスキップ
			if (hitTargets_.find(enemy->GetID()) != hitTargets_.end()) {
				continue;
			}

			// 敵にダメージを与える
			OnHitTarget(enemy.get());
			hitTargets_.insert(enemy->GetID());

			// 貫通回数が0になったら弾を無効化
			if (penetrationCount_ <= 0) {
				isActive_ = false;
			}
		}
	}
}

void Projectile::Draw() {
	if (!isActive_) {
		return;
	}

	// WVP行列をバッファにコピー
	render_->CopyBufferData(0, &wvp_, sizeof(Matrix4x4));

	// 色を指定（ライフタイムに応じてアルファ値を変更）
	float alpha = lifetime_ / maxLifetime_;
	Vector4 color = { 1.0f, 1.0f, 1.0f, alpha };
	render_->CopyBufferData(1, &color, sizeof(Vector4));
}