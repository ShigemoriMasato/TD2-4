#pragma once
#include <Render/RenderObject.h>
#include <SHEngine.h>
#include <assets/Model/ModelManager.h>

namespace Player{
class LevelUI {
public:
	// 初期化
	void Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, SHEngine::Input* input);

	// 更新
	void Update(Matrix4x4 vpMatrix, float deltaTime, float currentEXP, float maxEXP);

	// 描画
	void Draw(CmdObj* cmdObj);

private:
	// 経験値ゲージの構造体
	struct EXPGauge {
		std::unique_ptr<SHEngine::RenderObject> render = nullptr;
		Matrix4x4 wvp;
		Transform transform;
	};

private:
	// 描画用オブジェクトの初期化
	void InitializeRenderObj(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, std::unique_ptr<SHEngine::RenderObject>& render);

	// 経験値ゲージのスケール変更
	void EXPGaugeScaleChange(float currentEXP, float maxEXP);

private:
	// モデルマネージャ
	SHEngine::ModelManager* modelManager_ = nullptr;

	// FPSObserver
	std::unique_ptr<FPSObserver> fpsObserver_ = nullptr;

	// 入力
	SHEngine::Input* input_ = nullptr;

	// 描画用変数
	EXPGauge expGaugeFill_; // 前面
	EXPGauge expGaugeBG_;   // 背景

	// ゲージの横幅
	float gaugeWidth_ = 5.0f;

	// ゲージの座標X
	float gaugePosX = -10.0f;
};
}