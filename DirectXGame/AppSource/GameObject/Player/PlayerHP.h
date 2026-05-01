#pragma once
#include "../AppSource/GameObject/EasingAnimation/AnimationBundle.h"
#include <Render/RenderObject.h>
#include <Render/Font/Text.h>
#include <SHEngine.h>
#include <assets/Model/ModelManager.h>

namespace Player {
class HP {
public:
	// 初期化関数
	void Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager);

	// 更新関数
	void Update(Matrix4x4 vpMatrix, float deltaTime, float currentHP, float maxHP);

	// 描画関数
	void Draw(CmdObj* cmdObj);

private:
	// HPバーの構造体
	struct HPBar {
		std::unique_ptr<SHEngine::RenderObject> render = nullptr;
		Matrix4x4 wvp;
		Transform transform;
	};

private:
	// HP変化時にHPバーのスケールを変化させる関数
	void HPBarScaleChange(float currentHP, float maxHP);

	// 描画用変数の初期化関数
	void InitializeRenderHPBar(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, std::unique_ptr<SHEngine::RenderObject>& render);

	// HPバー　減った量のアニメーション
	void AnimationHPBarAfter(float deltaTime);

private:
	// 描画用変数
	HPBar hpBarFill_;  // 前面
	HPBar hpBarAfter_; // 減った分
	HPBar hpBarBG_;    // 背景

	// 説明用のUI
	std::unique_ptr<SHEngine::RenderObject> uiRender_ = nullptr;

	// モデルマネージャ
	SHEngine::ModelManager* modelManager_ = nullptr;

	// HPバー初期座標
	Vector2 hpBarPos_ = { 145.0f, -55.0f };

	// HPバーのサイズ
	Vector2 hpBarSize_ = { 200.0f, 50.0f };

	// HPバー　減った分のアニメーション用変数
	AnimationBundle<float> scaleAnimationHPBarAfter_;

	// 変化検知用
	float previousHP_ = -1.0f;

	// HPテキスト描画用変数
	std::unique_ptr<SHEngine::Text> hpText_ = nullptr;
	Transform hpTextTransform_ = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {70.0f, -65.0f, 0.0f} };
	float hpTextSize_ = 1.0f;
	Vector4 hpTextColor_ = { 1.0f, 1.0f, 1.0f, 1.0f };
	Vector4 hpColor_ = {0.4f, 0.0f, 0.0f, 1.0f};
};
} // namespace Player