#pragma once
#include "IAttackObject.h"
#include <Render/RenderObject.h>
#include <SHEngine.h>
#include <memory>

/// <summary>
/// ピストルの弾クラス
/// </summary>
class Projectile : public IAttackObject {
public:
	// コンストラクタ
	Projectile(const DamageInfo& damageInfo, const Vector3& position, const Vector3& direction, float speed);

	~Projectile() override;

	// 初期化関数
	void Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager);

	// 更新処理
	void Update(float deltaTime) override;

	// 描画処理
	void Draw() override;

	// 敵との当たり判定をチェックする関数
	void CheckCollisionWithEnemies(const std::vector<std::unique_ptr<Enemy>>& enemies);

	// 弾が有効かどうか
	bool IsActive() const { return isActive_; }

private:
	// Transform
	Transform transform_;

	// 飛行方向と速度
	Vector3 direction_ = { 0.0f, 0.0f, 1.0f };
	float speed_ = 20.0f;

	// ライフタイム
	float lifetime_ = 10.0f;
	float maxLifetime_ = 10.0f;

	// 当たり判定用の判定用円
	float collisionRadius_ = 0.5f;

	// 有効フラグ
	bool isActive_ = true;

	// 描画オブジェクト
	std::unique_ptr<SHEngine::RenderObject> render_ = nullptr;

	// WVP行列
	Matrix4x4 wvp_;
};