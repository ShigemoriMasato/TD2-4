#pragma once
#include <string>
#include <variant>
#include <Utility/Vector.h>

enum class ParticleType
{
	// ターゲットに向かって移動するやつ
	GoToTarget,
	// ビルボード可能・Scale/Rotate/Translate/ColorをPhysics操作可能
	B_S_R_T_C,

	None
};

inline const char* ToString(ParticleType t)
{
	switch (t)
	{
	case ParticleType::GoToTarget: return "GoToTarget";
	case ParticleType::B_S_R_T_C: return "B_S_R_T_C";
	default: return "Unknown";
	}
}

inline bool FromString(const std::string& s, ParticleType& out)
{
	if (s == "GoToTarget") { out = ParticleType::GoToTarget; return true; }
	else if (s == "B_S_R_T_C") { out = ParticleType::B_S_R_T_C; return true; }
	return false;
}

struct ParticleSRTComponent
{
	bool isRandom = false;
	Vector3 baseValue = { 0.0f, 0.0f, 0.0f };
	Vector3 randomRange_min = { 0.0f, 0.0f, 0.0f };
	Vector3 randomRange_max = { 0.0f, 0.0f, 0.0f };
};

struct ParticleSRTComponentFloat4
{
	bool isRandom = false;
	Vector4 baseValue = { 0.0f, 0.0f, 0.0f, 0.0f };
	Vector4 randomRange_min = { 0.0f, 0.0f, 0.0f, 0.0f };
	Vector4 randomRange_max = { 0.0f, 0.0f, 0.0f, 0.0f };
};

struct ParticleSRT
{
	ParticleSRTComponent value;
	ParticleSRTComponent velocity;
	ParticleSRTComponent acceleration;
};

struct ParticleSRTfloat4
{
	ParticleSRTComponentFloat4 value;
	ParticleSRTComponentFloat4 velocity;
	ParticleSRTComponentFloat4 acceleration;
};



struct ParticleConfig
{
	float lifeTime = 1.0f;
	float speed = 1.0f;

	int emitNum = 10;
	float emitInterval = 0.1f;

	bool isBillboard_ = false;

	std::string texturePath = "Assets/.EngineResource/Texture/white1x1.png";
	std::string modelPath = "Assets/.EngineResource/Model/Cube";
};

struct GoToTargetConfig
{
	ParticleConfig cfg{};

	bool isMoveToTarget = false;
	Vector3 TargetPos = { 0.0f, 0.0f, 0.0f };
	float moveSpeed = 1.0f;
};

struct B_S_R_T_C_Config
{
	ParticleConfig cfg{};
	bool billboard = false;
	ParticleSRT scale;
	ParticleSRT rotate;
	ParticleSRT translate;
	ParticleSRTfloat4 color;
};


using ParticlePresetVariant = std::variant<
	GoToTargetConfig,
	B_S_R_T_C_Config
>;