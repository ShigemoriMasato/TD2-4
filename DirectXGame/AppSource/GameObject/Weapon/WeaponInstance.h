#pragma once
#include "AttackObject/IAttackObject.h"
#include "DamageInfo.h"
#include "WeaponData.h"
#include <../Engine/SHEngine.h>
#include <../Engine/Utility/Vector.h>
#include <memory>
#include <vector>

class Enemy;
class EnemyManager;

/// <summary>
/// 武器のインスタンスクラス
/// </summary>
class WeaponInstance {
public:
	// コンストラクタ
	WeaponInstance(const WeaponData* data) : weaponData_(data) {
		cooldownDuration_ = weaponData_->attackSpeed; // 攻撃速度をクールダウンの初期値に設定
		currentCooldown_ = 0.0f;                      // クールダウンタイマーを初期化
	}

	// 初期化関数
	void Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager);

	// 更新処理
	void Update(const Vector3& playerPos, float deltaTime);

	// EnemyManagerを設定
	void SetEnemyManager(EnemyManager* enemyManager) { enemyManager_ = enemyManager; }

	// ダメージ情報を計算
	DamageInfo CalculateDamageInfo() const;

	// 攻撃オブジェクトを取得
	const std::vector<std::unique_ptr<IAttackObject>>& GetAttackObjects() const { return attackObjects_; }

	// 攻撃オブジェクトを手動で追加（テスト用）
	void AddAttackObject(std::unique_ptr<IAttackObject> attackObject);

private:
	// プレイヤーに最も近い敵を探索する処理
	Enemy* FindNearestEnemy(const Vector3& playerPos);

	// プレイヤーに最も近い敵を攻撃する処理
	void ExecuteAttack(const Vector3& playerPos);

	// 無効な攻撃オブジェクトを削除
	void CleanupInactiveObjects();

private:
	const WeaponData* weaponData_;                              // 武器データへのポインタ
	float currentCooldown_;                                     // クールダウン管理
	float cooldownDuration_;                                    // クールダウン時間
	EnemyManager* enemyManager_ = nullptr;                      // 敵のマネージャ
	std::vector<std::unique_ptr<IAttackObject>> attackObjects_; // 攻撃オブジェクトのリスト
	SHEngine::ModelManager* modelManager_ = nullptr;
	SHEngine::DrawDataManager* drawDataManager_ = nullptr;
};
