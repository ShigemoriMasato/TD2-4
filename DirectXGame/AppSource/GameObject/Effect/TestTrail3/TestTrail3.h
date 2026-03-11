#pragma once
#include <GameObject/Effect/Trail.h>

class TestTrail3
{
public:
	void Initialize(SHEngine::DrawDataManager* drawDataManager, SHEngine::TextureManager* textureManager);
	void Update(float deltaTime);
	void Draw(CmdObj* cmdObj, const Matrix4x4& vpMatrix);

private:
	Trail trail_;
	float time_ = 0.0f;
};
