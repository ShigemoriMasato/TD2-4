#pragma once
#include "GameObject/EasingAnimation/EasingAnimation.h"
#include <Render/RenderObject.h>
#include <assets/Model/ModelManager.h>
#include <memory>

/// <summary>
/// リザルト画面等で2つの剣が交差するアニメーションと描画を行うクラス
/// </summary>
class ResultSword {
public:
	// 初期化処理
	void Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, SHEngine::TextureManager* textureManager);

	// アニメーションの開始
	void StartAnimation();

	// 毎フレームの更新（アニメーション進行と行列転送）
	void Update(Matrix4x4 vpMatrix, float deltaTime);

	// 描画処理
	void Draw(CmdObj* cmdObj);

	// アニメーションが終了したかどうか
	bool IsAnimationFinished() const;

private:
	// 描画オブジェクト
	std::unique_ptr<SHEngine::RenderObject> renderSword1_;
	std::unique_ptr<SHEngine::RenderObject> renderSword2_;

	// 各剣のトランスフォーム
	Transform transformSword1_;
	Transform transformSword2_;

	// テクスチャインデックス
	int textureIndex_ = 0;

	// アニメーション管理
	EasingAnimation<Vector3> sword1PosAnim_;
	EasingAnimation<Vector3> sword1RotAnim_;
	EasingAnimation<Vector3> sword2PosAnim_;
	EasingAnimation<Vector3> sword2RotAnim_;

	// アニメーション完了フラグ
	bool isAnimationFinished_ = true; // 生成直後は終了状態として扱う
};