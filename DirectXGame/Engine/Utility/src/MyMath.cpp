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

float MyMath::lerp(float a, float b, float t)
{
	return a + (b - a) * t;
}

uint32_t MyMath::lerp(uint32_t start, uint32_t end, float t)
{
	int sa = start & 0xff;
	start >>= 8;
	int sb = start & 0xff;
	start >>= 8;
	int sg = start & 0xff;
	start >>= 8;
	int sr = start & 0xff;

	int ea = end & 0xff;
	end >>= 8;
	int eb = end & 0xff;
	end >>= 8;
	int eg = end & 0xff;
	end >>= 8;
	int er = end & 0xff;

	int aa = static_cast<int>(sa + (ea - sa) * t) << 24;
	int ab = static_cast<int>(sb + (eb - sb) * t) << 16;
	int ag = static_cast<int>(sg + (eg - sg) * t) << 8;
	int ar = static_cast<int>(sr + (er - sr) * t);

	return aa | ab | ag | ar;
}

Vector3 MyMath::lerp(const Vector3& a, const Vector3& b, float t)
{
	return Vector3(
		lerp(a.x, b.x, t),
		lerp(a.y, b.y, t),
		lerp(a.z, b.z, t)
	);
}

Quaternion MyMath::lerp(const Quaternion& a, const Quaternion& b, float t) {
	return Quaternion(
		lerp(a.x, b.x, t),
		lerp(a.y, b.y, t),
		lerp(a.z, b.z, t),
		lerp(a.w, b.w, t)
	);
}

float MyMath::EaseIn(float a, float b, float t)
{
	return a + (b - a) * t * t * t; // Cubic ease-in
}

Vector3 MyMath::EaseIn(Vector3 a, Vector3 b, float t)
{
	return Vector3(
		a.x + (b.x - a.x) * t * t * t,
		a.y + (b.y - a.y) * t * t * t,
		a.z + (b.z - a.z) * t * t * t
	);
}

float MyMath::EaseOut(float a, float b, float t)
{
	t = 1.0f - t; // Invert t for ease-out
	return a + (b - a) * (1.0f - t * t * t); // Cubic ease-out
}

Vector3 MyMath::EaseOut(Vector3 a, Vector3 b, float t)
{
	t = 1.0f - t; // Invert t for ease-out
	return Vector3(
		a.x + (b.x - a.x) * (1.0f - t * t * t),
		a.y + (b.y - a.y) * (1.0f - t * t * t),
		a.z + (b.z - a.z) * (1.0f - t * t * t)
	);
}

Vector3 MyMath::Slerp(Vector3 a, Vector3 b, float t)
{
	if (t <= 0.0f) return a;
	if (t >= 1.0f) return b;
	float dotProduct = std::clamp(dot(a, b), 0.0f, 1.0f);
	float theta = std::acosf(dotProduct);
	float sinTheta = std::sinf(theta);
	if (sinTheta < 1e-6f)
	{
		return a;
	}
	float ratioA = std::sinf((1.0f - t) * theta) / sinTheta;
	float ratioB = std::sinf(t * theta) / sinTheta;
	return Vector3(
		a.x * ratioA + b.x * ratioB,
		a.y * ratioA + b.y * ratioB,
		a.z * ratioA + b.z * ratioB
	);
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
