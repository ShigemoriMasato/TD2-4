#include "RibbonTrail.h"

void RibbonTrail::Initialize(SHEngine::TextureManager* textureManager, const RibbonTrailConfig& preset)
{
	preset_ = preset;
	trail_.Initialize(textureManager);
	trail_.SetConfig(preset_.cfg);
	trail_.Clear();
}

void RibbonTrail::Update(float dt)
{
	if (isActive_)
	{
		const Vector3 originWS = preset_.originLocal * modelWorld_;
		const Vector3 tipWS = preset_.tipLocal * modelWorld_;
		trail_.PushSegment(originWS, tipWS);
	}

	trail_.Update(dt);
}