#pragma once
#include <GameObject/Effect/Trail/Trail.h>
#include <GameObject/Effect/Trail/Preset/TrailPreset.h>

class RibbonTrail
{
public:
	void Initialize(SHEngine::TextureManager* textureManager, const RibbonTrailConfig& preset);
	void Update(float dt);

	void SetModelWorld(const Matrix4x4& modelWorld) { modelWorld_ = modelWorld; }
	void SetEnabled(bool isActive) { isActive_ = isActive; }

	void Clear() { trail_.Clear(); }

	Trail& GetTrail() { return trail_; }
	const Trail& GetTrail() const { return trail_; }

private:
	Trail trail_;
	RibbonTrailConfig preset_{};
	Matrix4x4 modelWorld_{ Matrix4x4::Identity() };
	bool isActive_ = true;
};