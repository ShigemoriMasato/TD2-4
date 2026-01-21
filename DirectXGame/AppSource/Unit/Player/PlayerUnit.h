#pragma once
#include"Unit/MapChipField.h"
#include"PlayerUnitObject.h"
#include"Common/KeyConfig/KeyManager.h"

#include"Collision/Collider.h"

class PlayerUnit : public Collider {
public:

	void Init(MapChipField* mapChipField, DrawData drawData,const Vector3& pos, KeyManager* keyManager);

	void Update();

	void Draw(Window* window, const Matrix4x4& vpMatrix);

	// 当たり判定
	void OnCollision(Collider* other) override;

public:

	Vector3* GetPos() { return &object_->transform_.position; }

private:

	// マップデータ
	MapChipField* mapChipField_ = nullptr;

	// オブジェクトデータ
	std::unique_ptr<PlayerUnitObject> object_;

	// キーを取得する
	KeyManager* keyManager_ = nullptr;

	// 体力
	int32_t hp_ = 0;

	// サイズ
	Vector3 size_ = { 0.8f,0.8f,0.8f };
	
	// 速度 
	Vector3 velocity_ = {};

	// 速さ
	float speed_ = 10.0f;

	// 円の当たり判定
	Circle circleCollider_;

	// デバック用
	std::string kGroupName_ = "Player";

private:

	// 入力処理
	void ProcessMoveInput();

	/// <summary>
	/// マップの衝突判定
	/// </summary>
	/// <param name="info"></param>
	void CheckMapCollision(MapChipField::CollisionMapInfo& info);

	/// <summary>
	/// 上方向の衝突判定
	/// </summary>
	/// <param name="info"></param>
	void CheckCollisionAbove(MapChipField::CollisionMapInfo& info);

	/// <summary>
	/// 下方向の衝突判定
	/// </summary>
	/// <param name="info"></param>
	void CheckCollisionBelow(MapChipField::CollisionMapInfo& info);

	/// <summary>
	/// 右方向の衝突判定
	/// </summary>
	/// <param name="info"></param>
	void CheckCollisionRight(MapChipField::CollisionMapInfo& info);

	/// <summary>
	/// 左方向の衝突判定
	/// </summary>
	/// <param name="info"></param>
	void CheckCollisionLeft(MapChipField::CollisionMapInfo& info);

private:

	/// <summary>
	/// 値を登録する
	/// </summary>
	void RegisterDebugParam();

	/// <summary>
	/// 値を適応する
	/// </summary>
	void ApplyDebugParam();
};