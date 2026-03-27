#include "FountainParticle.h"

void FountainParticle::Initialize(SHEngine::DrawDataManager* drawDataManager, SHEngine::TextureManager* textureManager, SHEngine::ModelManager* modelManager, const FountainConfig& preset)
{
	preset_ = preset;
	particle_.Initialize(drawDataManager, textureManager, modelManager, preset_.cfg);
}

void FountainParticle::Update(float dt, const Matrix4x4& vpMatrix)
{
	particle_.Update(dt, vpMatrix);
}

void FountainParticle::Draw(CmdObj* cmdObj)
{
	particle_.Draw(cmdObj);
}

void FountainParticle::Trigger(const Vector3& position)
{
	particle_.Trigger(position);
}

void FountainParticle::Stop()
{
	particle_.Stop();
}

void FountainParticle::Clear()
{
	particle_.Clear();
}