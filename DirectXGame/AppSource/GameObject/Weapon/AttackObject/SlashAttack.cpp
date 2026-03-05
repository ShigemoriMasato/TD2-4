#include "SlashAttack.h"
#include "../AppSource/GameObject/Random/Random.h"
#include <Utility/MatrixFactory.h>

using namespace SHEngine;

SlashAttack::SlashAttack(const WeaponData& weaponData, const Vector3& spawnPos, 
                         const Vector3& direction, SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager)
	: position_(spawnPos), direction_(direction), moveSpeed_(weaponData.range * 5.0f) {
	
	float finalDamage = weaponData.baseDamage;

	// クリティカル判定
	float criticalRand = RandomUtils::RangeFloat(0.0f, 1.0f);
	if (criticalRand < weaponData.criticalChance) {
		finalDamage *= weaponData.criticalMultiplier;
	}

	// ダメージ情報を設定
	damageInfo_.damage = finalDamage;
	damageInfo_.criticalChance = weaponData.criticalChance;
	damageInfo_.criticalMult = weaponData.criticalMultiplier;
	damageInfo_.knockbackPower = weaponData.knockbackPower;
	damageInfo_.lifeStealChance = weaponData.lifeStealChance;
	
	// 斬撃が存在する時間
	lifeTime_ = 0.5f;

	// トランスフォーム設定
	transform_.position = position_;
	transform_.rotate = { 0.0f, 0.0f, 0.0f };
	transform_.scale = { 1.0f, 1.0f, 1.0f };

	// WVP行列を初期化
	wvp_ = Matrix4x4::Identity();
	color_ = { 1.0f, 1.0f, 1.0f, 1.0f };

	// モデルと描画データの取得
	int modelHandle = modelManager->LoadModel("Assets/.EngineResource/Model/cube");
	auto modelData = modelManager->GetNodeModelData(modelHandle);
	auto drawData = drawDataManager->GetDrawData(modelData.drawDataIndex);

	render_ = std::make_unique<RenderObject>();
	render_->Initialize();

	// シェーダー設定
	render_->psoConfig_.vs = "Simple.VS.hlsl";
	render_->psoConfig_.ps = "White.PS.hlsl";
	render_->SetUseTexture(true);

	// CBVの生成
	render_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER);

	// 描画データを設定
	render_->SetDrawData(drawData);
}

void SlashAttack::Update(float deltaTime, Matrix4x4 vpMatrix) {
	// 寿命を減らす
	lifeTime_ -= deltaTime;

	// 斬撃を移動させる処理
	Vector3 move = direction_ * moveSpeed_ * deltaTime;
	position_ += move;

	// トランスフォーム更新
	transform_.position = position_;

	// WVP行列を計算（Update時点では vpMatrix を使わないため、ワールド行列のみ計算）
	wvp_ = Matrix::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.position);
	wvp_ *= vpMatrix;

	// 寿命に応じてアルファ値を変更（フェードアウト）
	float alpha = lifeTime_ / 0.5f;
	if (alpha < 0.0f) {
		alpha = 0.0f;
	}
	color_ = { 1.0f, 1.0f, 1.0f, alpha };
}

void SlashAttack::Draw(CmdObj* cmdObj) {
	// 生存時間が0より大きい場合のみ描画
	if (lifeTime_ > 0.0f && render_) {
		// バッファデータをコピー
		render_->CopyBufferData(0, &wvp_, sizeof(Matrix4x4));
		
		// 描画
		render_->Draw(cmdObj);
	}
}
