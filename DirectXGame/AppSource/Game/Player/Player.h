#pragma once
#include "State/IPlayerState.h"
#include "State/PlayerStateDash.h"
#include "State/PlayerStateNormal.h"
#include <Render/RenderObject.h>
#include <SHEngine.h>
#include <Tool/FPS/FPSObserver.h>
#include <assets/Model/ModelManager.h>

/// <summary>
/// プレイヤー
/// </summary>
class Player {
public:
	// 初期化
	void Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, SHEngine::Input* input);

	// 更新
	void Update(Matrix4x4 vpMatrix);

	// 描画
	void Draw(CmdObj* cmdObj);

	// 状態管理関数
	void ChangeState(std::unique_ptr<IPlayerState> newState);

	// StateからアクセスするためのGetter等
	SHEngine::Input* GetInput() const { return input_; }
	Transform& GetTransform() { return transform_; }
	void SetTransform(Transform t) { transform_ = t; }
	float GetVelocity() const { return velocity_; }

	bool CanDash() const { return dashCooldownTimer_ <= 0.0f; }
	void StartDashCooldown() { dashCooldownTimer_ = dashCooldown_; }
	void UpdateDashCooldown();

	void SetDashDir(Vector2 dir) { dashDir_ = dir; }
	Vector2 GetDashDir() const { return dashDir_; }
	float GetDashSpeed() const { return dashSpeed_; }
	float GetDashDuration() const { return dashDuration_; }

private:
	// 描画用変数
	std::unique_ptr<SHEngine::RenderObject> render_ = nullptr;

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
	float dashSpeed_ = 15.0f;        // ダッシュスピード
	float dashDuration_ = 0.15f;     // ダッシュ時間
	float dashCooldown_ = 0.5f;      // ダッシュのクールダウン
	float dashCooldownTimer_ = 0.0f; // クールダウン用のタイマー
	Vector2 dashDir_ = {0.0f, 0.0f}; // ダッシュする方向

	// FPSObserver
	std::unique_ptr<FPSObserver> fpsObserver_ = nullptr;
};
