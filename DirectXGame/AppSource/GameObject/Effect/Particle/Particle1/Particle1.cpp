#include "Particle1.h"

#include <algorithm>
#include <cmath>
#include <numbers>

#include <Utility/Matrix.h>
#include <GameObject/Random/Random.h>

using namespace SHEngine;

namespace
{
	float Lerp(float a, float b, float t) { return a + (b - a) * t; }

	Vector4 Lerp(const Vector4& a, const Vector4& b, float t)
	{
		return Vector4(
			Lerp(a.x, b.x, t),
			Lerp(a.y, b.y, t),
			Lerp(a.z, b.z, t),
			Lerp(a.w, b.w, t));
	}

	float Length(const Vector3& v) { return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z); }

	Vector3 Normalize(const Vector3& v)
	{
		const float len = Length(v);
		if (len <= 1e-6f) return Vector3(0.0f, 1.0f, 0.0f);
		return Vector3(v.x / len, v.y / len, v.z / len);
	}
}

void Particle1::Initialize()
{
	renderObject_ = std::make_unique<RenderObject>("Particle1_Fountain");
	renderObject_->Initialize();

	// インスタンス描画用（vp + world行列 + 色）
	renderObject_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "VP");
	renderObject_->CreateSRV(sizeof(Matrix4x4), maxParticles_, ShaderType::VERTEX_SHADER, "WorldMatrix");
	renderObject_->CreateSRV(sizeof(Vector4), maxParticles_, ShaderType::PIXEL_SHADER, "Colors");

	particles_.clear();
	particles_.reserve(maxParticles_);

	worlds_.resize(maxParticles_);
	colors_.resize(maxParticles_);

	emitAcc_ = 0.0f;
	isEmitting_ = false;
}

void Particle1::Trigger(const Vector3& pos)
{
	origin_ = pos;
	isEmitting_ = true;

	// ここでは既存粒を消さない（「位置を変えて再噴出」も可能）
	// もし「毎回リセットしたい」なら particles_.clear(); を追加
}

void Particle1::Stop()
{
	// 新規生成だけ止める（既存粒はUpdateで寿命まで残す）
	isEmitting_ = false;

	// 端数蓄積はリセットしておくと再開時が自然
	emitAcc_ = 0.0f;
}

void Particle1::Spawn(int count)
{
	for (int n = 0; n < count; ++n)
	{
		if ((int)particles_.size() >= maxParticles_)
			return;

		PrticleUnit unit{};
		unit.age = 0.0f;
		unit.life = RandomUtils::RangeFloat(minLife_, maxLife_);
		unit.pos = origin_;

		// 上向き円錐に散らす
		const float theta = RandomUtils::RangeFloat(0.0f, std::numbers::pi_v<float> *2.0f);
		const float r = RandomUtils::RangeFloat(0.0f, spread_);

		Vector3 dir = Normalize(Vector3(std::cos(theta) * r, 1.0f, std::sin(theta) * r));
		const float spd = RandomUtils::RangeFloat(minSpeed_, maxSpeed_);
		unit.vel = Vector3(dir.x * spd, dir.y * spd, dir.z * spd);

		particles_.push_back(unit);
	}
}

void Particle1::Update(float deltaTime)
{
	if (deltaTime <= 0.0f) return;

	// 生成（Stop中は生成しない）
	if (isEmitting_)
	{
		emitAcc_ += emitRate_ * deltaTime;
		const int spawnCount = (int)emitAcc_;
		emitAcc_ -= (float)spawnCount;

		if (spawnCount > 0)
			Spawn(spawnCount);
	}

	// 更新（寿命で消える）
	for (auto& u : particles_)
	{
		u.age += deltaTime;

		u.vel.y -= gravity_ * deltaTime;

		u.pos.x += u.vel.x * deltaTime;
		u.pos.y += u.vel.y * deltaTime;
		u.pos.z += u.vel.z * deltaTime;

		// 地面（origin_.y）を下回ったら寿命終了扱い（残るのは寿命まで、の思想に合う簡易）
		if (u.pos.y < origin_.y)
		{
			u.age = u.life;
		}
	}

	particles_.erase(
		std::remove_if(particles_.begin(), particles_.end(),
			[](const PrticleUnit& u) { return u.age >= u.life; }),
		particles_.end());
}

void Particle1::Draw(CmdObj* cmdObj, const Matrix4x4& vpMatrix)
{
	if (!renderObject_) return;

	const int count = (int)std::min(particles_.size(), (size_t)maxParticles_);

	// ブロック/水しぶき風（好みで調整）
	const Vector4 startColor(1.0f, 1.0f, 1.0f, 0.95f);
	const Vector4 endColor(1.0f, 1.0f, 1.0f, 0.0f);

	for (int i = 0; i < count; ++i)
	{
		const auto& u = particles_[i];
		const float t = std::clamp(u.age / u.life, 0.0f, 1.0f);

		// 徐々に小さく
		const float size = Lerp(0.30f, 0.08f, t);

		worlds_[i] = Matrix::MakeAffineMatrix(
			Vector3(size, size, size),
			Vector3(0.0f, 0.0f, 0.0f),
			u.pos);

		colors_[i] = Lerp(startColor, endColor, t);
	}

	renderObject_->CopyBufferData(0, &vpMatrix, sizeof(Matrix4x4));
	renderObject_->CopyBufferData(1, worlds_.data(), sizeof(Matrix4x4) * count);
	renderObject_->CopyBufferData(2, colors_.data(), sizeof(Vector4) * count);

	renderObject_->instanceNum_ = count;
	renderObject_->Draw(cmdObj);
}