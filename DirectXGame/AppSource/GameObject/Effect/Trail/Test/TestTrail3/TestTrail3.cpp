#include "TestTrail3.h"

#include <cmath>
#include <algorithm>
#include <numbers>

namespace
{
	// 0..1 -> 0..1（端がなだらか）
	float Smoothstep01(float x)
	{
		x = std::clamp(x, 0.0f, 1.0f);
		return x * x * (3.0f - 2.0f * x);
	}

	// 簡易ハッシュノイズ（0..1）。角度インデックスごとに固定の乱れを作る用途。
	float Hash01(int i)
	{
		// そこそこ分布する適当な定数
		float x = std::sin((float)i * 12.9898f) * 43758.5453f;
		return x - std::floor(x);
	}
}


void TestTrail3::Initialize(SHEngine::DrawDataManager* drawDataManager, SHEngine::TextureManager* textureManager)
{
	Trail::Config cfg{};
	cfg.maxSegments = maxSegments_;
	cfg.lifeTime = 0.20f;
	cfg.minDistance = 0.0f;
	cfg.texturePath = "Assets/.EngineResource/Texture/uvChecker.png";
	trail_.Initialize(textureManager, cfg);

	active_ = false;
	time_ = 0.0f;
}

void TestTrail3::Trigger(const Vector3& centerWS)
{
	centerWS_ = centerWS;
	active_ = true;
	time_ = 0.0f;
	trail_.Clear();
}

void TestTrail3::Update(float deltaTime, const Matrix4x4& vpMatrix)
{
	if (!active_) return;

	time_ += deltaTime;
	trail_.Clear();

	const float rad = radius_ * time_ * 2.0f;
	const float pi = std::numbers::pi_v<float>;
	for (int i = 0; i < maxSegments_; ++i)
	{
		const float angle = i * pi * 2.0f / maxSegments_;
		const float sin = std::sin(angle);
		const float cos = std::cos(angle);

		// base と tip
		const float x = centerWS_.x + cos * rad;
		const float z = centerWS_.z + sin * rad;
		const float y = i % 2 == 0 ? centerWS_.y : centerWS_.y + 0.5f;
		Vector3 baseWS = { x, centerWS_.y, z };
		Vector3 tipWS = { x, y, z };

		trail_.PushSegment(baseWS, tipWS);
	}

	trail_.Update(deltaTime, vpMatrix);

	if (time_ >= 0.5f)
	{
		active_ = false;
		trail_.Clear();
	}
}
