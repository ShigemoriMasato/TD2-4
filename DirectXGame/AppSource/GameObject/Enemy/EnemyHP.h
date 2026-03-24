#pragma once
#include "../AppSource/GameObject/EasingAnimation/AnimationBundle.h"
#include <Render/RenderObject.h>
#include <SHEngine.h>
#include <assets/Model/ModelManager.h>

class EnemyHP {
public:
	void Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager);

	void Update(Matrix4x4 vpMatrix, float deltaTime, float currentHP, float maxHP, const Vector2& enemyScreenPos);

	void Draw(CmdObj* cmdObj);

private:
	struct HPBar {
		std::unique_ptr<SHEngine::RenderObject> render = nullptr;
		Matrix4x4 wvp;
		Transform transform;
	};

	void HPBarScaleChange(float currentHP, float maxHP);
	void InitializeRenderHPBar(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, std::unique_ptr<SHEngine::RenderObject>& render);
	void AnimationHPBarAfter(float deltaTime);

	HPBar hpBarFill_;
	HPBar hpBarAfter_;
	HPBar hpBarBG_;

	Vector2 hpBarSize_ = {60.0f, 10.0f};
	Vector2 hpBarPos_ = {0.0f, 0.0f};

	SHEngine::ModelManager* modelManager_ = nullptr;
	AnimationBundle<float> scaleAnimationHPBarAfter_;
	float previousHP_ = -1.0f;
};