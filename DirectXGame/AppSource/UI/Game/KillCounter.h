#pragma once
#include <Render/Font/Text.h>
#include <Render/RenderObject.h>
#include <assets/Model/ModelManager.h>
#include "GameObject/EasingAnimation/AnimationBundle.h"

class KillCounter {
public:
	void Initialize(SHEngine::ModelManager* modelManager, SHEngine::DrawDataManager* drawDataManager, SHEngine::TextureManager* textureManager);
	void Update(float deltaTime, Matrix4x4 vpMatrix, int killCount);
	void Draw(CmdObj* cmdObj);

private:
	std::unique_ptr<SHEngine::RenderObject> enemySprite_;
	std::unique_ptr<SHEngine::Text> counterText_;
	Transform spriteTransform_;
	Transform textTransform_;
	int textureIndex_ = 0;
	AnimationBundle<float> scaleAnim_;
	float baseScale_ = 1.0f;
	int prevKillCount_ = 0;
};
