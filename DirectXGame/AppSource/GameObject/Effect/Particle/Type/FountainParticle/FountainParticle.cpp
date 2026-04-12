#include "FountainParticle.h"

void FountainParticle::Initialize(SHEngine::TextureManager* textureManager, SHEngine::ModelManager* modelManager)
{
	particle_.Initialize(textureManager, modelManager);
}

void FountainParticle::SetConfig(const FountainConfig& config)
{
	config_ = config;
	particle_.SetConfig(config_.cfg);
}

void FountainParticle::Update(float dt)
{
	particle_.Update(dt);
}
