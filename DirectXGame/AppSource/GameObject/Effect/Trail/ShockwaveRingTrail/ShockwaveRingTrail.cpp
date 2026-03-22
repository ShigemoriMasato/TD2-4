#include "ShockwaveRingTrail.h"
#include <cmath>
#include <algorithm>
#include <numbers>

void ShockwaveRingTrail::Initialize(SHEngine::DrawDataManager* drawDataManager, SHEngine::TextureManager* textureManager, const ShockwaveRingConfig& preset)
{
	preset_ = preset;
	trail_.Initialize(drawDataManager, textureManager, preset_.cfg);
	trail_.SetTexturePath(preset_.cfg.defaultTexturePath);
	trail_.Clear();

	active_ = false;
	time_ = 0.0f;
	position_ = { 0.0f, 1.0f, 0.0f };
}

void ShockwaveRingTrail::Trigger(const Vector3& position)
{
	position_ = position;

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

	const int seg = std::max(3, preset_.segments);
	const float twoPi = std::numbers::pi_v<float> *2.0f;

	for (int i = 0; i < seg; ++i)
	{
		const float a = (float)i / (float)seg * twoPi;
		const float s = std::sin(a);
		const float c = std::cos(a);

		const float noise = (preset_.noiseAmp <= 0.0f) ? 0.0f : (Hash01_(i) * 2.0f - 1.0f) * preset_.noiseAmp;
		const float r = std::max(0.0f, radius + noise);

		const Vector3 pos = position_ + Vector3(c * r, 0.0f, s * r);

		// 法線は常に上向き(0,1,0)で、そこから幅方向のベクトルを求める
		Vector3 widthDir = Vector3(-s, c, 0.0f).Normalize() * (preset_.thickness * 0.5f);

		Vector3 baseWS = pos - widthDir * (preset_.thickness * 0.5f);
		Vector3 tipWS = pos + widthDir * (preset_.thickness * 0.5f);

		baseWS = baseWS * modelWorld_;
		tipWS = tipWS * modelWorld_;

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