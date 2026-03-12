#pragma once
#include <Render/RenderObject.h>
#include <SHEngine.h>
#include <assets/Model/ModelManager.h>
#include <GameObject/Weapon/IWeapon.h>
#include "GameObject/EasingAnimation/AnimationBundle.h"

class IWeaponRender {
public:
	// 初期化関数
	void Initialize(SHEngine::DrawDataManager* drawDataManager, SHEngine::ModelManager* modelManager, IWeapon* weapon, Item itemData);

	// 更新関数
	void Update(Matrix4x4 vpMatrix, Vector3 playerPos, float deltaTime);

	// 描画関数
	void Draw(CmdObj* cmdObj);

	// Setter
	void SetPosition(Vector3 position) { transform_.position = position; }
	void SetRotate(Vector3 rotate) { transform_.rotate = rotate; }
	void SetScale(Vector3 scale) { transform_.scale = scale; }
	void SetWVP(Matrix4x4 wvp) { wvp_ = wvp; }
	void SetRotationMatrix(Matrix4x4 rotMatrix) { rotationMatrix_ = rotMatrix; }

	Matrix4x4 LookAt(const Vector3& direction, const Vector3& up);

	Piece* GetPiecePtr() const { return weapon_->GetPiecePtr(); }

protected:
	static inline int nextID_ = 0;
	int id_ = nextID_++;

	// テクスチャのインデックス
	int textureIndex_;

	// 描画用変数
	std::unique_ptr<SHEngine::RenderObject> render_ = {};

	// WVP行列
	Matrix4x4 wvp_ = {};

	// トランスフォーム
	Transform transform_ = {};

	IWeapon* weapon_;

	Matrix4x4 rotationMatrix_ = Matrix4x4::Identity();

	// アニメーション用の変数
	AnimationBundle<Vector3> bundle_;
};
