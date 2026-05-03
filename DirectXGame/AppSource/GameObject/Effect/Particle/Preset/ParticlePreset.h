#pragma once
#include <GameObject/Effect/Particle/Particle.h>
#include <string>

enum class ParticleType
{
	// 物理型(速度、加速度で動くやつ)
	Physics,
	// ターゲットに向かって移動するやつ
	GoToTarget,
	// エミッターがトレイルなやつ
	OnTrail,
	// 拡大→縮小→消滅(ビルボード)
	Billboard_Scale,
	// 生成→拡散→縮小→消滅(ビルボード)
	Billboard_Scale2, //

	None
};

inline const char* ToString(ParticleType t)
{
	switch (t)
	{
	case ParticleType::Physics: return "Physics";
	case ParticleType::OnTrail: return "OnTrail";
	case ParticleType::GoToTarget: return "GoToTarget";
	case ParticleType::Billboard_Scale: return "Billboard_Scale";
	case ParticleType::Billboard_Scale2: return "Billboard_Scale2";
	default: return "Unknown";
	}
}

inline bool FromString(const std::string& s, ParticleType& out)
{
	if (s == "Physics") { out = ParticleType::Physics; return true; }
	else if (s == "GoToTarget") { out = ParticleType::GoToTarget; return true; }
	else if (s == "OnTrail") { out = ParticleType::OnTrail; return true; }
	else if (s == "Billboard_Scale") { out = ParticleType::Billboard_Scale; return true; }
	else if (s == "Billboard_Scale2") { out = ParticleType::Billboard_Scale2; return true; }
	return false;
}


#pragma region PhysicsConfig

struct ParticleSRTComponent
{
	bool isRandom = false;
	Vector3 baseValue = { 0.0f, 0.0f, 0.0f };
	Vector3 randomRange_min = { 0.0f, 0.0f, 0.0f };
	Vector3 randomRange_max = { 0.0f, 0.0f, 0.0f };
};

struct ParticleSRT
{
	ParticleSRTComponent value;
	ParticleSRTComponent velocity;
	ParticleSRTComponent acceleration;
};

struct PhysicsConfig
{
	Particle::Config cfg{};

	ParticleSRT scale;
	ParticleSRT rotate;
	ParticleSRT translate;
};

#pragma endregion 

#pragma region GoToTargetConfig

struct GoToTargetConfig
{
	Particle::Config cfg{};

	bool isMoveToTarget = false;
	Vector3 TargetPos = { 0.0f, 0.0f, 0.0f };
	float moveSpeed = 1.0f;
};

#pragma endregion

#pragma region OnTrailConfig

struct OnTrailConfig
{
	Particle::Config cfg{};
};

#pragma endregion

#pragma region BillboardScaleConfig

struct BillboardScaleConfig
{
	Particle::Config cfg{};

	ParticleSRT scale;
};

#pragma endregion

#pragma region BillboardScale2Config

struct BillboardScale2Config
{
	Particle::Config cfg{};
	ParticleSRT scale;
};

#pragma endregion