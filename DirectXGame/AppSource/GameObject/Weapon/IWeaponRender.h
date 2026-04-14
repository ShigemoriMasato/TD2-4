#pragma once
#include "GameObject/EasingAnimation/AnimationBundle.h"
#include <GameObject/Effect/Trail/DataBank/TrailPresetDataBank.h>
#include <GameObject/Effect/Trail/MultiTrail/MultiTrail.h>
#include <GameObject/Weapon/IWeapon.h>
#include <Render/RenderObject.h>
#include <SHEngine.h>
#include <assets/Model/ModelManager.h>
#include <Scene/CommonData.h>

class IWeaponRender {
public:
	// 初期化関数
	void Initialize(
		SHEngine::DrawDataManager* drawDataManager, SHEngine::ModelManager* modelManager, SHEngine::TextureManager* textureManager, IWeapon* weapon, Item itemData, const std::string& trailname, CommonData& commonData);

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

	// アニメーションの状態管理用
	enum class AnimState { None, Forward, Return };

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

	// アニメーションの状態
	AnimState animState_ = AnimState::None;

	// 前フレームのアニメーション状態
	bool prevIsAnimation_ = false;

	// 武器の向く方向
	float direction_;

	// アニメーション用の変数群
	AnimationBundle<Vector3> posOffsetAnim_;   // 座標
	AnimationBundle<Vector3> rotOffsetAnim_;   // 回転
	AnimationBundle<Vector3> scaleOffsetAnim_; // スケール

	// Pickaxe用の状態管理変数
	bool isPickaxeThrust_ = true;     // 次のアニメーションが突きかどうか
	bool currentAnimIsThrust_ = true; // 現在実行中のアニメーションが突きかどうか

	// トレイル
	MultiTrail trail_;
};
