#pragma once
#include <GameObject/Effect/Particle/Particle.h>
#include <string>

enum class ParticleType
{
	// 噴水型
	Fountain,
	// ターゲットに向かって移動するやつ
	GoToTarget,
	// エミッターがトレイルなやつ
	OnTrail,

	None
};

inline const char* ToString(ParticleType t)
{
	switch (t)
	{
	case ParticleType::Fountain: return "Fountain";
	case ParticleType::OnTrail: return "OnTrail";
	default: return "Unknown";
	}
}

inline bool FromString(const std::string& s, ParticleType& out)
{
	if (s == "Fountain") { out = ParticleType::Fountain; return true; }
	else if (s == "GoToTarget") { out = ParticleType::Fountain; return true; }
	else if (s == "OnTrail") { out = ParticleType::OnTrail; return true; }
	return false;
}

struct VectorDynamics
{
	Vector3 value = { 1.0f,1.0f,1.0f };
	Vector3 velocity;
	Vector3 acceleration;
};

struct ParticleSRT
{
	VectorDynamics initial;

	bool isRandom_value = false;
	Vector3 randomRange_value_min;
	Vector3 randomRange_value_max;

	bool isRandom_velocity = false;
	Vector3 randomRange_velocity_min;
	Vector3 randomRange_velocity_max;

	bool isRandom_acceleration = false;
	Vector3 randomRange_acceleration_min;
	Vector3 randomRange_acceleration_max;
};



struct FountainConfig
{
	Particle::Config cfg{};

	ParticleSRT scale;
	ParticleSRT rotate;
	ParticleSRT translate;
};

struct GoToTargetConfig
{
	Particle::Config cfg{};

	bool isMoveToTarget = false;
	Vector3 TargetPos = { 0.0f, 0.0f, 0.0f };
	float moveSpeed = 1.0f;
};

struct OnTrailConfig
{
	Particle::Config cfg{};
};
