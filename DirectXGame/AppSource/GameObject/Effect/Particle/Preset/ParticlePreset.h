#pragma once
#include <string>
#include <variant>
#include <Utility/Vector.h>

enum class ParticleType
{
	// 物理型(速度、加速度で動くやつ)
	Physics,
	// ターゲットに向かって移動するやつ
	GoToTarget,
	// エミッターがトレイルなやつ
	OnTrail,
	// 拡縮→scale0以下で消滅(ビルボード)
	B_S,
	// 生成→拡散→拡縮→scale0以下で消滅(ビルボード)
	B_S_T,
	// 透明化&拡縮→color.w0以下で消滅(ビルボード)
	B_S_C,
	// ビルボード可能・Scale/Rotate/Translate/ColorをPhysics操作可能
	B_S_R_T_C,

	None
};

inline const char* ToString(ParticleType t)
{
	switch (t)
	{
	case ParticleType::Physics: return "Physics";
	case ParticleType::OnTrail: return "OnTrail";
	case ParticleType::GoToTarget: return "GoToTarget";
	case ParticleType::B_S: return "B_S";
	case ParticleType::B_S_T: return "B_S_T";
	case ParticleType::B_S_C: return "B_S_C";
	case ParticleType::B_S_R_T_C: return "B_S_R_T_C";
	default: return "Unknown";
	}
}

inline bool FromString(const std::string& s, ParticleType& out)
{
	if (s == "Physics") { out = ParticleType::Physics; return true; }
	else if (s == "GoToTarget") { out = ParticleType::GoToTarget; return true; }
	else if (s == "OnTrail") { out = ParticleType::OnTrail; return true; }
	else if (s == "B_S") { out = ParticleType::B_S; return true; }
	else if (s == "B_S_T") { out = ParticleType::B_S_T; return true; }
	else if (s == "B_S_C") { out = ParticleType::B_S_C; return true; }
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

	std::string texturePath = "Assets/.EngineResource/Texture/white1x1.png";
	std::string modelPath = "Assets/.EngineResource/Model/Cube";
};

#pragma region PhysicsConfig

struct PhysicsConfig
{
	ParticleConfig cfg{};

	ParticleSRT scale;
	ParticleSRT rotate;
	ParticleSRT translate;
};

#pragma endregion 

#pragma region GoToTargetConfig

struct GoToTargetConfig
{
	ParticleConfig cfg{};

	bool isMoveToTarget = false;
	Vector3 TargetPos = { 0.0f, 0.0f, 0.0f };
	float moveSpeed = 1.0f;
};

#pragma endregion

#pragma region OnTrailConfig

struct OnTrailConfig
{
	ParticleConfig cfg{};
};

#pragma endregion

#pragma region B_S_Config

struct B_S_Config
{
	ParticleConfig cfg{};

	ParticleSRT scale;
};

#pragma endregion

#pragma region B_S_T_Config

struct B_S_T_Config
{
	ParticleConfig cfg{};
	ParticleSRT scale;
};

#pragma endregion

#pragma region B_S_C_Config

struct B_S_C_Config
{
	ParticleConfig cfg{};
	ParticleSRT scale;
	ParticleSRTfloat4 color;
};

#pragma endregion

#pragma region B_S_R_T_C_Config

struct B_S_R_T_C_Config
{
	ParticleConfig cfg{};
	bool billboard = false;
	ParticleSRT scale;
	ParticleSRT rotate;
	ParticleSRT translate;
	ParticleSRTfloat4 color;
};

#pragma endregion

using ParticlePresetVariant = std::variant<
	PhysicsConfig,
	OnTrailConfig,
	GoToTargetConfig,
	B_S_Config,
	B_S_T_Config,
	B_S_C_Config,
	B_S_R_T_C_Config
>;