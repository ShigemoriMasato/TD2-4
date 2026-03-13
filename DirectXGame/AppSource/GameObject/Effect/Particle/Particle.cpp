#include "Particle.h"
#include "GameObject/Effect/Particle/Particle1/Particle1.h"

void Particle::SetType(ParticleType type)
{
	type_ = type;

	switch (type_)
	{
	case ParticleType::None:
		particle_.reset();
		break;

	case ParticleType::Test1: particle_ = std::make_unique<Particle1>(); break;
		// case ParticleType::Test2: particle_ = std::make_unique<Particle2>(); break;
		// case ParticleType::Test3: particle_ = std::make_unique<Particle3>(); break;

	default:
		particle_.reset();
		break;
	}
}

void Particle::Initialize()
{
	if (particle_) particle_->Initialize();
}

void Particle::Update(float deltaTime)
{
	if (particle_) particle_->Update(deltaTime);
}

void Particle::Draw(CmdObj* cmdObj, const Matrix4x4& vpMatrix)
{
	if (particle_) particle_->Draw(cmdObj, vpMatrix);
}