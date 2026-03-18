#pragma once
#include <GameObject/Effect/Trail/Trail.h>
#include <GameObject/Effect/Trail/Preset/TrailPreset.h>

class RibbonTrail
{
public:
	void Initialize(SHEngine::DrawDataManager* drawDataManager, SHEngine::TextureManager* textureManager, const RibbonTrailPreset& preset);
	void Update(float dt, const Matrix4x4& vpMatrix);
	void Draw(CmdObj* cmdObj);

	void SetModelWorld(const Matrix4x4& modelWorld) { modelWorld_ = modelWorld; }

	void SetEnabled(bool enabled) { enabled_ = enabled; }


private:
	Trail trail_;
	RibbonTrailPreset preset_{};
	Matrix4x4 modelWorld_{ Matrix4x4::Identity() };
	bool enabled_ = true;
};