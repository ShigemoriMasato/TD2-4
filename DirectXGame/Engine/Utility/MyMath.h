#pragma once
#include <cstdint>
#include <vector>
#include "../Quaternion.h"
#include "../Matrix.h"
#include "../Vector.h"

namespace MyMath {
	float lerp(float a, float b, float t);

	/// <summary>
	/// RGBAを線形補完
	/// </summary>
	/// <param name="a"></param>
	/// <param name="b"></param>
	/// <param name="t"></param>
	/// <returns></returns>
	uint32_t lerp(uint32_t a, uint32_t b, float t);
	Vector3 lerp(const Vector3& a, const Vector3& b, float t);
	Quaternion lerp(const Quaternion& a, const Quaternion& b, float t);

	float EaseIn(float a, float b, float t);
	Vector3 EaseIn(Vector3 a, Vector3 b, float t);

	float EaseOut(float a, float b, float t);
	Vector3 EaseOut(Vector3 a, Vector3 b, float t);

	Vector3 Slerp(Vector3 a, Vector3 b, float t);

	float Repeat(float value, float max, float min = 0.0f);

	Vector3 ConvertVector(const Vector4& v);
	Vector4 ConvertVector(const Vector3& v, float w);

	float cot(float radian);

	Vector3 Normalize(Vector3 vec);

	Vector3 cross(const Vector3& a, const Vector3& b);
	float dot(const Vector3& a, const Vector3& b);
	float Length(const Vector3& vec);

	std::vector<Vector3> GetSplinePoints(const std::vector<Vector3>& controlPoints, int segment);
	std::vector<Vector3> GetCatmullPoints(const std::vector<Vector3>& points, int segmentsPerCurve);
	Vector3 GetCatmullPoint(const std::vector<Vector3>& points, float t);
	float CalculateAngle(const Vector3& a, const Vector3& b);

}
