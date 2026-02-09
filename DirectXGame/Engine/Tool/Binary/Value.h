#pragma once
#include <cstdint>
#include <string>

struct DirectionalLight;

enum class TypeID : uint8_t {
	kUnknown = 0x00,	//不明な型

	//もともとある型
	Int = 0x01,
	Float,
	Bool,
	String,
	Double,
	uint8_t,
	uint16_t,
	uint32_t,

	//VectorとかMatrixとか
	Vector2 = 0x10,
	Vector3,
	Vector4,

	Vector = 0x40,

	//構造体とか
	Custom = 0x80,
};

struct Vector2;
struct Vector3;
struct Vector4;

template<typename T>
struct TypeIDResolver {
	static constexpr TypeID id = TypeID::kUnknown;
};

template<> struct TypeIDResolver<int> { static constexpr TypeID id = TypeID::Int; };
template<> struct TypeIDResolver<float> { static constexpr TypeID id = TypeID::Float; };
template<> struct TypeIDResolver<bool> { static constexpr TypeID id = TypeID::Bool; };
template<> struct TypeIDResolver<std::string> { static constexpr TypeID id = TypeID::String; };
template<> struct TypeIDResolver<double> { static constexpr TypeID id = TypeID::Double; };
template<> struct TypeIDResolver<uint8_t> { static constexpr TypeID id = TypeID::uint8_t; };
template<> struct TypeIDResolver<uint16_t> { static constexpr TypeID id = TypeID::uint16_t; };
template<> struct TypeIDResolver<uint32_t> { static constexpr TypeID id = TypeID::uint32_t; };
template<> struct TypeIDResolver<Vector2> { static constexpr TypeID id = TypeID::Vector2; };
template<> struct TypeIDResolver<Vector3> { static constexpr TypeID id = TypeID::Vector3; };
template<> struct TypeIDResolver<Vector4> { static constexpr TypeID id = TypeID::Vector4; };
