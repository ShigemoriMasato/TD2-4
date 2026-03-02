#pragma once
#include "../AppSource/Game/Player/Status/StatusManager.h"
#include "State/IPlayerState.h"
#include "State/PlayerStateDash.h"
#include "State/PlayerStateNormal.h"
#include <Render/RenderObject.h>
#include <SHEngine.h>
#include <assets/Model/ModelManager.h>

/// <summary>
/// プレイヤー
/// </summary>
namespace Player {
struct AfterImage {
	Transform transform;
	float timer;
};

class Base {
public:
	// 初期化
	void Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, SHEngine::Input* input);

	// 更新
	void Update(Matrix4x4 vpMatrix, float deltaTime);

	// 描画
	void Draw(CmdObj* cmdObj);

	// 状態管理関数
	void ChangeState(std::unique_ptr<IPlayerState> newState);

	// 残像を追加する関数
	void SpawnAfterImage();

	// 残像の更新処理
	void UpdateAfterImages(float deltaTime);

	// StateからアクセスするためのGetter等
	SHEngine::Input* GetInput() const { return input_; }
	Transform& GetTransform() { return transform_; }
	void SetTransform(Transform t) { transform_ = t; }
	float GetVelocity() const { return velocity_; }

	bool CanDash() const { return dashCooldownTimer_ <= 0.0f; }
	void StartDashCooldown() { dashCooldownTimer_ = dashCooldown_; }
	void UpdateDashCooldown(float deltaTime);

	void SetDashDir(Vector2 dir) { dashDir_ = dir; }
	Vector2 GetDashDir() const { return dashDir_; }
	float GetDashSpeed() const { return dashSpeed_; }
	float GetDashDuration() const { return dashDuration_; }

	void SetPosition(Vector3 position) { transform_.position = position; }
	void SetRotate(Vector3 rotate) { transform_.rotate = rotate; }

	float GetRotationSpeed() const { return rotationSpeed_; }

	float GetMaxHP() const { return statusManager_->GetStatusValue(StatusType::MaxHP); }

private:
	// 描画用変数
	std::unique_ptr<SHEngine::RenderObject> render_ = nullptr;

	// 残像描画用変数
	std::unique_ptr<SHEngine::RenderObject> afterImageRender_ = nullptr;

	// WVP行列
	Matrix4x4 wvp_;

	// 入力
	SHEngine::Input* input_ = nullptr;

	// Transform
	Transform transform_{};

	// 速度
	float velocity_ = 5.0f;

	// 現在の状態
	std::unique_ptr<IPlayerState> currentState_ = nullptr;

	// ダッシュ用のパラメータ
	float dashSpeed_ = 30.0f;        // ダッシュスピード
	float dashDuration_ = 0.15f;     // ダッシュ時間
	float dashCooldown_ = 0.5f;      // ダッシュのクールダウン
	float dashCooldownTimer_ = 0.0f; // クールダウン用のタイマー
	Vector2 dashDir_ = {0.0f, 0.0f}; // ダッシュする方向

	// 残像関連の変数
	std::vector<AfterImage> afterImages_; // 残像のステータス
	float afterImageLifeTime_ = 0.3f;     // 残像が画面に残る時間

	// VP行列
	Matrix4x4 vpMatrix_;

	// テクスチャのインデックス
	int textureIndex_;

	// 回転の滑らかさ
	float rotationSpeed_ = 10.0f;

	// 残像・インスタンスの最大値
	static const int kMaxInstanceAfterImage = 8;

	// ステータスマネージャ
	std::unique_ptr<StatusManager> statusManager_ = nullptr;
};
} // namespace Player
