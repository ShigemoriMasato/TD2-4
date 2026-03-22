#include "RibbonTrail.h"

void RibbonTrail::Initialize(SHEngine::DrawDataManager* drawDataManager, SHEngine::TextureManager* textureManager, const RibbonTrailConfig& preset)
{
	preset_ = preset;
	trail_.Initialize(drawDataManager, textureManager, preset_.cfg);
	trail_.SetTexturePath(preset_.cfg.defaultTexturePath);
	trail_.Clear();
}

void RibbonTrail::Update(float dt, const Matrix4x4& vpMatrix)
{
	if (enabled_)
	{
		const Vector3 originWS = preset_.originLocal * modelWorld_;
		const Vector3 tipWS = preset_.tipLocal * modelWorld_;
		trail_.PushSegment(originWS, tipWS);
	}

	trail_.Update(dt, vpMatrix);
}

void RibbonTrail::Draw(CmdObj* cmdObj)
{
	trail_.Draw(cmdObj);
}