#pragma once
#include <GameObject/Effect/Particle/Particle.h>
#include <string>

enum class ParticleType
{
	// 噴水型
	Fountain,
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
	else if (s == "OnTrail") { out = ParticleType::OnTrail; return true; }
	return false;
}

struct FountainConfig
{
	Particle::Config cfg{};

};

struct OnTrailConfig
{
	Particle::Config cfg{};

};