#pragma once
#include <Render/RenderObject.h>
#include <SHEngine.h>
#include <assets/Model/ModelManager.h>

class Player;

class PlayerHP {
public:
	// 初期化関数
	void Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, SHEngine::Input* input);

	// 更新関数
	void Update(Matrix4x4 vpMatrix);

	// 描画関数
	void Draw(CmdObj* cmdObj);

	// ダメージ関数
	void Damage(float amount);

	// 回復関数
	void Heal(float amount);

	// Getter等
	float GetCurrentHP() { return currentHP_; }

private:
	// HPバーの構造体
	struct HPBar{
		std::unique_ptr<SHEngine::RenderObject> render = nullptr;
		Matrix4x4 wvp;
		Transform transform;
	};

private:
	// HP変化時にHPバーのスケールを変化させる関数
	void HPBarScaleChange();

	// 無敵状態のカウントダウン
	void InvincibleTimerUpdate();

	// 描画用変数の初期化関数
	void InitializeRenderHPBar(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, std::unique_ptr<SHEngine::RenderObject>& render);

private:
	// 描画用変数
	HPBar hpBarFill_; // 前面
	HPBar hpBarAfter_; // 減った分
	HPBar hpBarBG_; // 背景

	// 現在のHP
	float currentHP_ = 5.0f;

	// HPの最大値
	float maxHP = currentHP_;

	// 無敵フラグ
	bool isInvincible_ = false;

	// 無敵時間
	float invincibleDuration_ = 1.0f;

	// 無敵時間のカウントダウン
	float invincibleTimer_ = invincibleDuration_;

	// FPSObserver
	std::unique_ptr<FPSObserver> fpsObserver_ = nullptr;

	// 入力
	SHEngine::Input* input_ = nullptr;

	// HPバーの横幅
	const float kHPBarWidth = 5.0f;
};
