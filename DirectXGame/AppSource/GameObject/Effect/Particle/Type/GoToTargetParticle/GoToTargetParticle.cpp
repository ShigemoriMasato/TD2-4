#include "GoToTargetParticle.h"

void GoToTargetParticle::SetConfig(const ParticlePresetVariant& config)
{
	config_ = config;
	const GoToTargetConfig& uniqueConfig = std::get<GoToTargetConfig>(config_);
	SetModel(uniqueConfig.cfg.modelPath);
	SetTexture(uniqueConfig.cfg.texturePath);
}

void GoToTargetParticle::Update(float dt)
{
	Clear();

	// 「いま生きてる粒が無い」かつ「emitも止まってる」なら何もしない
	if (!isActive_ && instances_.empty()) return;

	const GoToTargetConfig& uniqueConfig = std::get<GoToTargetConfig>(config_);

	// 一定時間ごとにパーティクルを発生させる
	if (isActive_ && (emitTimer_ >= uniqueConfig.cfg.emitInterval || emitTimer_ < 0.0f))
	{
		emitTimer_ = 0.0f;
		for (int32_t i = 0; i < uniqueConfig.cfg.emitNum; ++i)
		{
			ParticleInstance ins{};
			ins.age = 0.0f;
			ins.color = Vector4(1, 1, 1, 1);
			ins.translate.value.baseValue = Vector3(0, 0, 0);
			instances_.push_back(ins);
		}
	}

	emitTimer_ += dt;
	for (auto& instance : instances_)
	{
		instance.age += dt;
		if (uniqueConfig.isMoveToTarget)
		{
			Vector3 dir = uniqueConfig.TargetPos - instance.translate.value.baseValue;
			float len = dir.Length();
			if (len > 0.01f)
			{
				dir = dir.Normalize();
				instance.translate.value.baseValue += dir * uniqueConfig.moveSpeed * dt;
			}
			else
			{
				instance.translate.value.baseValue = uniqueConfig.TargetPos;
			}
		}
	}

	// 寿命切れ削除
	instances_.erase(
		std::remove_if(instances_.begin(), instances_.end(),
			[&uniqueConfig](const ParticleInstance& p) { return p.age >= uniqueConfig.cfg.lifeTime; }),
		instances_.end());
}