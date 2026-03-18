#include "ShockwaveRingTrail.h"
#include <cmath>
#include <algorithm>
#include <numbers>

void ShockwaveRingTrail::Initialize(SHEngine::DrawDataManager* drawDataManager, SHEngine::TextureManager* textureManager, const ShockwaveRingPreset& preset)
{
	preset_ = preset;
	trail_.Initialize(drawDataManager, textureManager, preset_.cfg);
	trail_.SetTexturePath(preset_.cfg.defaultTexturePath);
	trail_.Clear();

	active_ = false;
	time_ = 0.0f;
	centerWS_ = {};
	normalWS_ = { 0.0f, 1.0f, 0.0f };
}

void ShockwaveRingTrail::Trigger(const Vector3& centerWS, const Vector3& normalWS)
{
	centerWS_ = centerWS;
	normalWS_ = NormalizeSafe_(normalWS);

	active_ = true;
	time_ = 0.0f;
	trail_.Clear();
}

void ShockwaveRingTrail::Stop()
{
	active_ = false;
	time_ = 0.0f;
	trail_.Clear();
}

Vector3 ShockwaveRingTrail::NormalizeSafe_(const Vector3& v)
{
	const float len = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
	if (len <= 1e-6f) return { 0.0f, 1.0f, 0.0f };
	return { v.x / len, v.y / len, v.z / len };
}

Vector3 ShockwaveRingTrail::Cross_(const Vector3& a, const Vector3& b)
{
	return Vector3(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	);
}

float ShockwaveRingTrail::Hash01_(int i)
{
	float x = std::sin((float)i * 12.9898f) * 43758.5453f;
	return x - std::floor(x);
}

void ShockwaveRingTrail::Update(float dt, const Matrix4x4& vpMatrix)
{
	if (!active_) return;

	time_ += dt;

	// 毎フレーム作り直す（TestTrail3方式）
	trail_.Clear();

	const float t = std::clamp(time_ / preset_.duration, 0.0f, 1.0f);
	// Smoothstep
	const float tt = t * t * (3.0f - 2.0f * t);
	const float radius = preset_.radiusStart + (preset_.radiusEnd - preset_.radiusStart) * tt;

	// 平面の基底（u,v）を作る
	const Vector3 n = normalWS_;
	Vector3 ref = (std::abs(n.y) > 0.99f) ? Vector3(0.0f, 0.0f, 1.0f) : Vector3(0.0f, 1.0f, 0.0f);
	Vector3 u = NormalizeSafe_(Cross_(ref, n));
	Vector3 v = NormalizeSafe_(Cross_(n, u));

	const int seg = std::max(3, preset_.segments);
	const float twoPi = std::numbers::pi_v<float> *2.0f;

	for (int i = 0; i < seg; ++i)
	{
		const float a = (float)i / (float)seg * twoPi;
		const float s = std::sin(a);
		const float c = std::cos(a);

		const float noise = (preset_.noiseAmp <= 0.0f) ? 0.0f : (Hash01_(i) * 2.0f - 1.0f) * preset_.noiseAmp;
		const float r = std::max(0.0f, radius + noise);

		const Vector3 pos = centerWS_ + u * (c * r) + v * (s * r);

		// 太さ方向：平面内で半径方向に直交（接線に近い）を使う
		Vector3 tangent = NormalizeSafe_((u * (-s)) + (v * (c)));
		Vector3 widthDir = tangent; // 見た目優先

		const Vector3 baseWS = pos - widthDir * (preset_.thickness * 0.5f);
		const Vector3 tipWS = pos + widthDir * (preset_.thickness * 0.5f);

		trail_.PushSegment(baseWS, tipWS);
	}

	trail_.Update(dt, vpMatrix);

	if (time_ >= preset_.duration)
	{
		Stop();
	}
}

void ShockwaveRingTrail::Draw(CmdObj* cmdObj)
{
	trail_.Draw(cmdObj);
}