#pragma once
#include <memory>
#include <Utility/Matrix.h>
#include "GameObject/Effect/Particle/IParticle.h"

class Particle
{
public:
	Particle() = default;

	// 種類選択 所詮enumなのでintを渡したってかまわない
	void SetType(ParticleType type);

	void Initialize();
	void Update(float deltaTime);
	void Draw(CmdObj* cmdObj, const Matrix4x4& vpMatrix);

	ParticleType GetType() const { return type_; }
	bool HasInstance() const { return particle_ != nullptr; }

private:
	ParticleType type_ = ParticleType::None;
	std::unique_ptr<IParticle> particle_;
};