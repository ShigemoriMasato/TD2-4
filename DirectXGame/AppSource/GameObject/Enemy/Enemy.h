#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <Utility/Vector.h>
#include <memory>

/// @brief 敵の種類
enum class EnemyType : int {
	Normal,  // 通常の敵
	Speed,   // 速い敵
	
	Count,
};

/// @brief 生成の種類
enum class SpawnType: int{
	Scatter,       // バラバラな個所に生成
	Concentration, // ある程度纏まって生成
};

/// @brief 敵の状態
enum class EnemyState : int {
	None, //
	Fire, // やけど状態

	Count,
};

/// @brief 敵のパラメータ
struct EnemyParam {
	int hp;     // 体力
	int attack; // Playerに与えるダメージ
	int speed;  // 移動速度
};

/// @brief 敵クラス
class Enemy {
public:

	Enemy();
	~Enemy();

	/// @brief 敵を初期化
	void Initialize();

	/// @brief 敵を更新
	/// @param deltaTime デルタタイム（秒）
	void Update(float deltaTime);

	/// @brief 敵を描画
	/// @param cmdObj コマンドオブジェクト
	void Draw();

	/// @brief 敵の生成設定
	/// @param type 敵の種類
	/// @param position 生成位置
	/// @param targetPosition ターゲット位置
	void Setup(EnemyType type, const Vector3& position, const Vector3& targetPosition);

	/// @brief 生存フラグを取得
	/// @return true: 生存, false: 死亡
	bool IsAlive() const { return isAlive_; }

	/// @brief ダメージを与える
	/// @param damage ダメージ量
	void TakeDamage(int damage);

	/// @brief 位置を取得
	/// @return 位置
	Vector3 GetPosition() const { return position_; }

	/// @brief 敵の種類を取得
	/// @return 敵の種類
	EnemyType GetType() const { return type_; }

	/// @brief 攻撃力を取得
	/// @return 攻撃力
	int GetAttack() const { return param_.attack; }

	/// @brief 状態を設定
	/// @param state 状態
	void SetState(EnemyState state) { state_ = state; }

	/// @brief 状態を取得
	/// @return 状態
	EnemyState GetState() const { return state_; }

	/// @brief 敵のID を取得
    /// @return 敵の一意なID
	int GetID() const { return id_; }
private:

	/// @brief 敵のタイプに応じたパラメータを取得
	/// @param type 敵の種類
	/// @return 敵のパラメータ
	EnemyParam GetEnemyParam(EnemyType type);

	/// @brief 移動処理
	/// @param deltaTime デルタタイム（秒）
	void Move(float deltaTime);

private:

	// パラメータ
	EnemyType type_ = EnemyType::Normal;
	EnemyParam param_{};
	EnemyState state_ = EnemyState::None;

	// 位置と移動
	Vector3 position_ = { 0.0f, 0.0f, 0.0f };
	Vector3 targetPosition_ = { 0.0f, 0.0f, 0.0f };
	Vector3 direction_ = { 0.0f, 0.0f, 0.0f };

	// 生存フラグ
	bool isAlive_ = false;

	// 生成後の待機時間（1秒間動かない）
	float spawnTimer_ = 0.0f;
	const float spawnWaitTime_ = 1.0f;
	bool canMove_ = false;

	// ID（一意に敵を識別するため）
	static int nextID_;
	int id_;
};