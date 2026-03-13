#pragma once
#include <GameObject/Effect/Trail/Trail.h>

class TestTrail1
{
public:
	void Initialize(SHEngine::DrawDataManager* drawDataManager, SHEngine::TextureManager* textureManager);
	void Update(float deltaTime, const Matrix4x4& vpMatrix);
	void Draw(CmdObj* cmdObj);

private:
	Trail trail_;
	float time_ = 0.0f;
};
