#include "B_S_Particle.h"
#include <Utility/MatrixFactory.h>

void B_S_Particle::SetConfig(const ParticlePresetVariant& config)
{
	config_ = config;

	const B_S_Config& uniqueConfig = std::get<B_S_Config>(config);
	SetModel(uniqueConfig.cfg.modelPath);
	SetTexture(uniqueConfig.cfg.texturePath);
}

void B_S_Particle::Update(float dt)
{
	Clear();

	// 「いま生きてる粒が無い」かつ「emitも止まってる」なら何もしない
	if (!isActive_ && instances_.empty()) return;

	const B_S_Config& uniqueConfig = std::get<B_S_Config>(config_);

	// 一定時間ごとにパーティクルを発生させる
	if (isActive_ && emitTimer_ >= uniqueConfig.cfg.emitInterval)
	{
		emitTimer_ = 0.0f;

		for (int32_t i = 0; i < uniqueConfig.cfg.emitNum; ++i)
		{
			ParticleInstance ins{};

			ins.age = 0.0f;
			ins.color = Vector4(1, 1, 1, 1);
			ins.scale = uniqueConfig.scale;

			instances_.push_back(ins);
		}
	}

	emitTimer_ += dt;

	// 年齢更新 & SRT更新
	for (auto& instance : instances_)
	{
		instance.scale.value.baseValue += instance.scale.velocity.baseValue * dt + 0.5f * instance.scale.acceleration.baseValue * dt * dt;
		instance.scale.velocity.baseValue += instance.scale.acceleration.baseValue * dt;

		instance.age += dt;
	}

	// 寿命切れ削除 & スケールがマイナスのものも削除
	instances_.erase(
		std::remove_if(instances_.begin(), instances_.end(),
			[&uniqueConfig](const ParticleInstance& p)
			{
				return
					p.age >= uniqueConfig.cfg.lifeTime ||
					p.scale.value.baseValue.x < 0.0f ||
					p.scale.value.baseValue.y < 0.0f ||
					p.scale.value.baseValue.z < 0.0f;
			}),
		instances_.end());


	// SRT更新 & GPU転送データ作成
	Vector3 myPos = Vector3(modelWorld_.m[3][0], modelWorld_.m[3][1], modelWorld_.m[3][2]);
	Vector3 cameraPos = cameraPos_;
	Vector3 toCameraDir = cameraPos - myPos;
	Vector3 normDir = toCameraDir.Normalize();
	float pitch = std::asinf(-normDir.y); // -sin(pitch) = y 成分
	float yaw = std::atan2f(normDir.x, normDir.z); // sin(yaw) = x 成分, cos(yaw) = z 成分
	Vector3 rotate(pitch, yaw, 0.0f); // roll はここでは未使用
	for (auto& instance : instances_)
	{
		const Matrix4x4 world = Matrix::MakeAffineMatrix(instance.scale.value.baseValue, rotate, myPos);

		pushInstance(world, instance.color);
	}
}
