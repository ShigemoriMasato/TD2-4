#include "RibbonTrail2Point.h"

void RibbonTrail2Point::Initialize(SHEngine::DrawDataManager* drawDataManager, SHEngine::TextureManager* textureManager, const Ribbon2PointPreset& preset)
{
	preset_ = preset;
	trail_.Initialize(drawDataManager, textureManager, preset_.cfg);
	trail_.SetTexturePath(preset_.cfg.defaultTexturePath);
	trail_.Clear();
}

void RibbonTrail2Point::Update(float dt, const Matrix4x4& vpMatrix)
{
	if (enabled_)
	{
		const Vector3 originWS = preset_.originLocal * modelWorld_;
		const Vector3 tipWS = preset_.tipLocal * modelWorld_;
		trail_.PushSegment(originWS, tipWS);
	}

	trail_.Update(dt, vpMatrix);
}

void RibbonTrail2Point::Draw(CmdObj* cmdObj)
{
	trail_.Draw(cmdObj);
}