#include "../MyMath.h"
#include <cmath>
#include <cassert>
#include <algorithm>
#include <numbers>

namespace
{
	bool ZeroChecker(float* value, int size)
	{
		bool isNotZero = true;

		for (int i = 0; i < size; ++i)
		{
			if (std::fabsf(value[i]) <= 1e-6)
			{
				isNotZero = false;
				break;
			}
		}

		return isNotZero;
	}

	Vector3 deCasteljau(const std::vector<Vector3>& controlPoints, float t) {
		if (controlPoints.size() == 1) return controlPoints[0];

		std::vector<Vector3> nextLevel;
		for (size_t i = 0; i < controlPoints.size() - 1; ++i) {
			Vector3 p = controlPoints[i] * (1 - t) + controlPoints[i + 1] * t;
			nextLevel.push_back(p);
		}

		return deCasteljau(nextLevel, t);
	}

	Vector3 catmullRom(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t) {
		float t2 = t * t;
		float t3 = t2 * t;

		return
			p0 * (-0.5f * t3 + t2 - 0.5f * t) +
			p1 * (1.5f * t3 - 2.5f * t2 + 1.0f) +
			p2 * (-1.5f * t3 + 2.0f * t2 + 0.5f * t) +
			p3 * (0.5f * t3 - 0.5f * t2);
	}

}

std::vector<Vector3> MyMath::GetSplinePoints(const std::vector<Vector3>& controlPoints, int segment)
{
	std::vector<Vector3> ans;
	for (int i = 0; i < segment; ++i)
	{
		float t = static_cast<float>(i) / static_cast<float>(segment);

		ans.push_back(deCasteljau(controlPoints, t));
	}

	return ans;
}

std::vector<Vector3> MyMath::GetCatmullPoints(const std::vector<Vector3>& points, int segmentsPerCurve)
{
	std::vector<Vector3> ans;

	if (points.size() < 4) return ans;

	for (size_t i = 1; i < points.size() - 2; ++i)
	{
		for (int j = 0; j <= segmentsPerCurve; ++j)
		{
			float t = static_cast<float>(j) / segmentsPerCurve;
			ans.push_back(catmullRom(points[i - 1], points[i], points[i + 1], points[i + 2], t));
		}
	}

	return ans;
}

Vector3 MyMath::GetCatmullPoint(const std::vector<Vector3>& points, float t)
{
	if (points.size() < 4)
	{
		assert(false && "Not enough points for Catmull-Rom interpolation");
		return Vector3();
	}
	float segmentLength = 1.0f / (points.size() - 3);
	int segmentIndex = static_cast<int>(t / segmentLength);
	segmentIndex = std::clamp(segmentIndex, 0, static_cast<int>(points.size()) - 4);
	float localT = (t - segmentIndex * segmentLength) / segmentLength;
	return catmullRom(points[segmentIndex], points[segmentIndex + 1], points[segmentIndex + 2], points[segmentIndex + 3], localT);
}

float MyMath::CalculateAngle(const Vector3& a, const Vector3& b)
{
	float dotProduct = dot(a,b);
	float magnitudeA = std::sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
	float magnitudeB = std::sqrt(b.x * b.x + b.y * b.y + b.z * b.z);

	if (magnitudeA == 0 || magnitudeB == 0) return (float)std::numbers::pi;

	float cosAngle = dotProduct / (magnitudeA * magnitudeB);
	cosAngle = std::clamp(cosAngle, -1.0f, 1.0f);

	return std::cos(cosAngle);
}

float MyMath::Repeat(float value, float max, float min)
{
	while (value < min || value > max)
	{
		if (value < min)
		{
			value += (max - min);
		}
		else if (value > max)
		{
			value -= (max - min);
		}
	}
	return value;
}

Vector3 MyMath::ConvertVector(const Vector4& v)
{
	return Vector3(v.x, v.y, v.z);
}

Vector4 MyMath::ConvertVector(const Vector3& v, float w)
{
	return Vector4(v.x, v.y, v.z, w);
}

float MyMath::cot(float radian)
{
	return std::cosf(radian) / std::sinf(radian);
}

Vector3 MyMath::Normalize(Vector3 vec)
{
	float length = std::sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
	return { vec.x / length, vec.y / length, vec.z / length };
}

Vector3 MyMath::cross(const Vector3& a, const Vector3& b)
{
	return Vector3(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	);
}

float MyMath::dot(const Vector3& a, const Vector3& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

float MyMath::Length(const Vector3& vec)
{
	return std::sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
}
