#include "B_S_R_T_C_Particle.h"
#include <Utility/MatrixFactory.h>
#include <GameObject/Random/Random.h>

namespace
{
	Vector3 RandInAABB(const Vector3& min, const Vector3& max)
	{
		return Vector3(
			RandomUtils::RangeFloat(min.x, max.x),
			RandomUtils::RangeFloat(min.y, max.y),
			RandomUtils::RangeFloat(min.z, max.z)
		);
	}

	Vector4 RandInAABB(const Vector4& min, const Vector4& max)
	{
		return Vector4(
			RandomUtils::RangeFloat(min.x, max.x),
			RandomUtils::RangeFloat(min.y, max.y),
			RandomUtils::RangeFloat(min.z, max.z),
			RandomUtils::RangeFloat(min.w, max.w)
		);
	}
	Vector3 Transformmm(const Vector3& vector, const Matrix4x4& matrix)
	{
		Vector3 result{};

		// ベクトルと行列の計算
		result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
		result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
		result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];
		float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];

		// wが0でなければ
		assert(w != 0.0f);
		result.x /= w;
		result.y /= w;
		result.z /= w;

		// 計算結果をVector3型で返す
		return result;
	}
}

void B_S_R_T_C_Particle::SetConfig(const ParticlePresetVariant& config)
{
	config_ = config;
	const B_S_R_T_C_Config& uniqueConfig = std::get<B_S_R_T_C_Config>(config_);
	SetModel(uniqueConfig.cfg.modelPath);
	SetTexture(uniqueConfig.cfg.texturePath);
}

void B_S_R_T_C_Particle::Update(float dt)
{
	// 「いま生きてる粒が無い」かつ「emitも止まってる」なら何もしない
	if (!isActive_ && instances_.empty()) return;

	Clear();

	const B_S_R_T_C_Config& uniqueConfig = std::get<B_S_R_T_C_Config>(config_);

	emitTimer_ += dt;

	// 一定時間ごとにパーティクルを発生させる
	if (isActive_ && (emitTimer_ >= uniqueConfig.cfg.emitInterval || emitTimer_ < 0.0f))
	{
		emitTimer_ = 0.0f;

		for (int32_t i = 0; i < uniqueConfig.cfg.emitNum; ++i)
		{
			ParticleSRT s;
			ParticleSRT r;
			ParticleSRT t;
			ParticleSRTfloat4 c;

			if (uniqueConfig.translate.value.isRandom) t.value.baseValue = RandInAABB(uniqueConfig.translate.value.randomRange_min, uniqueConfig.translate.value.randomRange_max);
			else t.value.baseValue = uniqueConfig.translate.value.baseValue;
			if (uniqueConfig.translate.velocity.isRandom) t.velocity.baseValue = RandInAABB(uniqueConfig.translate.velocity.randomRange_min, uniqueConfig.translate.velocity.randomRange_max);
			else t.velocity.baseValue = uniqueConfig.translate.velocity.baseValue;
			if (uniqueConfig.translate.acceleration.isRandom) t.acceleration.baseValue = RandInAABB(uniqueConfig.translate.acceleration.randomRange_min, uniqueConfig.translate.acceleration.randomRange_max);
			else t.acceleration.baseValue = uniqueConfig.translate.acceleration.baseValue;

			if (uniqueConfig.rotate.value.isRandom) r.value.baseValue = RandInAABB(uniqueConfig.rotate.value.randomRange_min, uniqueConfig.rotate.value.randomRange_max);
			else r.value.baseValue = uniqueConfig.rotate.value.baseValue;
			if (uniqueConfig.rotate.velocity.isRandom) r.velocity.baseValue = RandInAABB(uniqueConfig.rotate.velocity.randomRange_min, uniqueConfig.rotate.velocity.randomRange_max);
			else r.velocity.baseValue = uniqueConfig.rotate.velocity.baseValue;
			if (uniqueConfig.rotate.acceleration.isRandom) r.acceleration.baseValue = RandInAABB(uniqueConfig.rotate.acceleration.randomRange_min, uniqueConfig.rotate.acceleration.randomRange_max);
			else r.acceleration.baseValue = uniqueConfig.rotate.acceleration.baseValue;

			if (uniqueConfig.scale.value.isRandom) s.value.baseValue = RandInAABB(uniqueConfig.scale.value.randomRange_min, uniqueConfig.scale.value.randomRange_max);
			else s.value.baseValue = uniqueConfig.scale.value.baseValue;
			if (uniqueConfig.scale.velocity.isRandom) s.velocity.baseValue = RandInAABB(uniqueConfig.scale.velocity.randomRange_min, uniqueConfig.scale.velocity.randomRange_max);
			else s.velocity.baseValue = uniqueConfig.scale.velocity.baseValue;
			if (uniqueConfig.scale.acceleration.isRandom) s.acceleration.baseValue = RandInAABB(uniqueConfig.scale.acceleration.randomRange_min, uniqueConfig.scale.acceleration.randomRange_max);
			else s.acceleration.baseValue = uniqueConfig.scale.acceleration.baseValue;

			if (uniqueConfig.color.value.isRandom) c.value.baseValue = RandInAABB(uniqueConfig.color.value.randomRange_min, uniqueConfig.color.value.randomRange_max);
			else c.value.baseValue = uniqueConfig.color.value.baseValue;
			if (uniqueConfig.color.velocity.isRandom) c.velocity.baseValue = RandInAABB(uniqueConfig.color.velocity.randomRange_min, uniqueConfig.color.velocity.randomRange_max);
			else c.velocity.baseValue = uniqueConfig.color.velocity.baseValue;
			if (uniqueConfig.color.acceleration.isRandom) c.acceleration.baseValue = RandInAABB(uniqueConfig.color.acceleration.randomRange_min, uniqueConfig.color.acceleration.randomRange_max);
			else c.acceleration.baseValue = uniqueConfig.color.acceleration.baseValue;

			instances_.push_back({ s,r,t,0.0f, c });
		}
	}

	// 年齢更新 & SRT更新 & 色更新
	for (auto& instance : instances_)
	{
		instance.translate.value.baseValue += instance.translate.velocity.baseValue * dt + 0.5f * instance.translate.acceleration.baseValue * dt * dt;
		instance.translate.velocity.baseValue += instance.translate.acceleration.baseValue * dt;
		instance.rotate.value.baseValue += instance.rotate.velocity.baseValue * dt + 0.5f * instance.rotate.acceleration.baseValue * dt * dt;
		instance.rotate.velocity.baseValue += instance.rotate.acceleration.baseValue * dt;
		instance.scale.value.baseValue += instance.scale.velocity.baseValue * dt + 0.5f * instance.scale.acceleration.baseValue * dt * dt;
		instance.scale.velocity.baseValue += instance.scale.acceleration.baseValue * dt;
		instance.color.value.baseValue += instance.color.velocity.baseValue * dt + 0.5f * instance.color.acceleration.baseValue * dt * dt;
		instance.color.velocity.baseValue += instance.color.acceleration.baseValue * dt;

		instance.age += dt;
	}

	// 寿命切れ削除 & マイナススケール削除 & アルファマイナス削除
	instances_.erase(
		std::remove_if(instances_.begin(), instances_.end(),
			[&uniqueConfig](const ParticleInstance& p)
			{
				return
					p.age >= uniqueConfig.cfg.lifeTime ||
					p.scale.value.baseValue.x < 0.0f ||
					p.scale.value.baseValue.y < 0.0f ||
					p.scale.value.baseValue.z < 0.0f ||
					p.color.value.baseValue.w < 0.0f;
			}),
		instances_.end());


	// GPU転送データ作成
	
	for (auto& instance : instances_)
	{
		// 色のクランプ
		instance.color.value.baseValue.x = std::clamp(instance.color.value.baseValue.x, 0.0f, 1.0f);
		instance.color.value.baseValue.y = std::clamp(instance.color.value.baseValue.y, 0.0f, 1.0f);
		instance.color.value.baseValue.z = std::clamp(instance.color.value.baseValue.z, 0.0f, 1.0f);
		instance.color.value.baseValue.w = std::clamp(instance.color.value.baseValue.w, 0.0f, 1.0f);

		// ワールド行列作成
		Matrix4x4 world;
		if (uniqueConfig.cfg.isBillboard_)
		{
			Vector3 localPos = instance.translate.value.baseValue;
			Vector3 worldPos = Transformmm(localPos, modelWorld_);
			Vector3 toCameraDir = cameraPos_ - worldPos;
			Vector3 normDir = toCameraDir.Normalize();
			float pitch = std::asinf(-normDir.y); // -sin(pitch) = y 成分
			float yaw = std::atan2f(normDir.x, normDir.z); // sin(yaw) = x 成分, cos(yaw) = z 成分
			Vector3 rotate(pitch, yaw, instance.rotate.value.baseValue.x); // roll はここでは未使用

			world = Matrix::MakeAffineMatrix(instance.scale.value.baseValue, rotate, worldPos);
		}
		else
		{
			world = Matrix::MakeAffineMatrix(instance.scale.value.baseValue, instance.rotate.value.baseValue, instance.translate.value.baseValue) * modelWorld_;
		}

		pushInstance(world, instance.color.value.baseValue);
	}
}

