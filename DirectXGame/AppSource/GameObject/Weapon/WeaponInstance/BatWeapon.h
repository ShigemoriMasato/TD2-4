#pragma once
#include "../AppSource/GameObject/Weapon/AttackObject/SlashAttack.h"
#include "../AppSource/GameObject/Weapon/WeaponController.h"
#include <Render/RenderObject.h>
#include <SHEngine.h>
#include <assets/Model/ModelManager.h>

class EnemyManager;

class BatWeapon : public BaseWeapon {
public:
	// コンストラクタ
	BatWeapon(const WeaponData& data, SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager);

	// 更新処理
	void Update(float deltaTime, WeaponController* controller, Matrix4x4 vpMatrix, Vector3 playerPos) override;

	// 描画処理
	void Draw(CmdObj* cmdObj) override;

	// EnemyManagerを設定
	void SetEnemyManager(EnemyManager* enemyManager) { enemyManager_ = enemyManager; }

protected:
	// 攻撃オブジェクトの生成
	void Attack(WeaponController* controller);

	// 最も近い敵の方向を取得
	Vector3 GetNearestEnemyDirection();

	// 描画用変数
	std::unique_ptr<SHEngine::RenderObject> render_;

	// テクスチャのインデックス
	int textureIndex_;

	// WVP行列
	Matrix4x4 wvp_ = {};

	// トランスフォーム
	Transform transform_ = {};

	SHEngine::ModelManager* modelManager_ = nullptr;
	SHEngine::DrawDataManager* drawDataManager_ = nullptr;
	EnemyManager* enemyManager_ = nullptr;
};
