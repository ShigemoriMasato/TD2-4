#pragma once
#include "GameObject/EasingAnimation/EasingAnimation.h"
#include <Render/RenderObject.h>
#include <assets/Model/ModelManager.h>

/// <summary>
/// バックパック内に武器を配置したときに生成されるエフェクト
/// </summary>
class GaugeAttractEffect {
public:
	void Initialize(const Vector3& start, const Vector3& end, SHEngine::DrawDataManager* drawDataManager, SHEngine::ModelManager* modelManager);
	void Update(Matrix4x4 vpMatrix, float deltaTime);
	void Draw(CmdObj* cmdObj);

	// 終了フラグ
	bool IsFinished() const { return isFinished_; }

private:
	Vector3 EvaluateBezier(float t) const;

private:
	std::unique_ptr<SHEngine::RenderObject> render_;
	Transform transform_;
	Vector3 startPos_;
	Vector3 endPos_;
	Vector3 control1_;
	Vector3 control2_;
	Vector3 offset1_ = {0.0f, -50.0f, 0.0f};
	Vector3 offset2_ = {0.0f, -50.0f, 0.0f};

	float time_ = 0.0f;
	float duration_ = 0.6f;
	bool isFinished_ = false;
	int textureIndex_ = 0;

	static const int kTrailCount = 6;
	float trailDelay_ = 0.03f;

	EaseType easeType_ = EaseType::EaseInOutCubic;
};