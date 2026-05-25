#pragma once
#include "GameObject/EasingAnimation/EasingAnimation.h"
#include <Render/RenderObject.h>
#include <SHEngine.h>
#include <assets/Model/ModelManager.h>

/// <summary>
/// バックパック内に武器を配置したときに生成されるエフェクト
/// </summary>
class GaugeAttractEffect {
public:
	void Initialize(SHEngine::DrawDataManager* drawDataManager, SHEngine::ModelManager* modelManager, SHEngine::TextureManager* textureManager, Vector3 startPos);
	void Update(Matrix4x4 vpMatrix, float deltaTime);
	void Draw(CmdObj* cmdObj);

	// 終了フラグ
	bool IsFinished() const { return isFinished_; }

private:
	Transform transform_;
	float alpha_ = 1.0f;
	bool isFinished_ = false; // アニメーション全体の終了フラグ

	std::unique_ptr<SHEngine::RenderObject> explosionRender_;
	Vector3 explosionScale_;
	int ringTexture_ = 0;
	int crossTexture_ = 0;

	EasingAnimation<Vector3> scaleAnim_;
	EasingAnimation<float> alphaAnim_;
	float currentScale_ = 0.0f;
	float currentAlpha_ = 0.0f;
};