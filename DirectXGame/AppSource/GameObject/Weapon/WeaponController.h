#pragma once
#include "WeaponData.h"
#include "WeaponInstance.h"
#include <../Engine/SHEngine.h>
#include <../Engine/Utility/Vector.h>
#include <memory>
#include <vector>

class EnemyManager;
class IAttackObject;

/// <summary>
/// 装備中の武器群を管理するクラス
/// </summary>
class WeaponController {
public:
	// 初期化
	void Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager);

	// 武器の追加
	void AddWeapon(const WeaponData* data);

	// 更新処理
	void Update(const Vector3& playerPos, float deltaTime);

	// EnemyManagerを設定
	void SetEnemyManager(EnemyManager* enemyManager);

	// 敵との当たり判定をチェック
	void CheckCollisionsWithEnemies();

	// 全ての攻撃オブジェクトを描画
	void DrawAllAttackObjects();

private:
	std::vector<std::unique_ptr<WeaponInstance>> activeWeapons_; // アクティブな武器のリスト
	EnemyManager* enemyManager_ = nullptr;                       // 敵管理クラスのポインタ
	SHEngine::ModelManager* modelManager_ = nullptr;
	SHEngine::DrawDataManager* drawDataManager_ = nullptr;
};