#include "FountainParticle.h"

void FountainParticle::Initialize(SHEngine::DrawDataManager* drawDataManager, SHEngine::TextureManager* textureManager, SHEngine::ModelManager* modelManager)
{
	particle_.Initialize(drawDataManager, textureManager, modelManager);
}

void FountainParticle::SetConfig(const FountainConfig& config)
{
	config_ = config;
	particle_.SetConfig(config_.cfg);
}

void FountainParticle::SetEmitPos(const Vector3& pos)
{
	particle_.SetEmitPos(pos);
}

void FountainParticle::SetEmittingFlag(bool flag)
{
	particle_.SetEmittingFlag(flag);
}

void FountainParticle::Update(float dt, const Matrix4x4& vpMatrix)
{
	particle_.Update(dt, vpMatrix);
}

void FountainParticle::Draw(CmdObj* cmdObj)
{
	particle_.Draw(cmdObj);
}

void FountainParticle::Clear()
{
	particle_.Clear();
}