#pragma once
#include "../AppSource/GameObject/Weapon/WeaponData.h"
#include "IAttackObject.h"
#include <Assets/Model/ModelManager.h>
#include <Render/RenderObject.h>
#include <Utility/Vector.h>
#include <memory>

class SlashAttack : public IAttackObject {
public:
	// 武器データと、発生位置・方向などを受け取って初期化
	SlashAttack(const WeaponData& weaponData, const Vector3& spawnPos, const Vector3& direction, SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager);

	// 更新処理
	void Update(float deltaTime, Matrix4x4 vpMatrix) override;

	// 描画処理
	void Draw(CmdObj* cmdObj) override;

	bool IsActive() const override { return lifeTime_ > 0.0f && penetrationCount_ >= 0; }

private:
	float lifeTime_;                                 // 斬撃の残り生存時間
	Vector3 position_;                               // 斬撃の現在位置
	Vector3 direction_;                              // 斬撃の移動方向
	float moveSpeed_;                                // 斬撃の移動速度
	std::unique_ptr<SHEngine::RenderObject> render_; // 描画用インスタンス
	Matrix4x4 wvp_;                                  // WVP行列
	Transform transform_;                            // トランスフォーム
	Vector4 color_;                                  // 色（アルファ値）
};
