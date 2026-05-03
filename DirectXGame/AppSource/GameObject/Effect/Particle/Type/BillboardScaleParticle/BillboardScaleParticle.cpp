#include "BillboardScaleParticle.h"
#include <Utility/MatrixFactory.h>

void BillboardScaleParticle::Initialize(SHEngine::TextureManager* textureManager, SHEngine::ModelManager* modelManager)
{
	particle_.Initialize(textureManager, modelManager);
}

void BillboardScaleParticle::SetConfig(const BillboardScaleConfig& config)
{
	config_ = config;
	particle_.SetModel(config.cfg.modelPath);
	particle_.SetTexture(config.cfg.texturePath);
}


void BillboardScaleParticle::Update(float dt)
{
	// 「いま生きてる粒が無い」かつ「emitも止まってる」なら何もしない
	if (!isActive_ && instances_.empty())
	{
		particle_.Clear();
		return;
	}

	particle_.Clear();

	// 一定時間ごとにパーティクルを発生させる
	if (isActive_ && emitTimer_ >= config_.cfg.emitInterval)
	{
		emitTimer_ = 0.0f;

		for (int32_t i = 0; i < config_.cfg.emitNum; ++i)
		{
			ParticleInstance ins{};

			ins.age = 0.0f;
			ins.color = Vector4(1, 1, 1, 1);
			ins.scale = config_.scale;

			instances_.push_back(ins);
		}
	}

	emitTimer_ += dt;

	// 年齢更新 & 老人削除
	for (auto& instance : instances_)
	{
		instance.age += dt;
	}
	instances_.erase(
		std::remove_if(instances_.begin(), instances_.end(),
			[this](const ParticleInstance& p) { return p.age >= config_.cfg.lifeTime; }),
		instances_.end());
	instances_.erase(
		std::remove_if(instances_.begin(), instances_.end(),
			[this](const ParticleInstance& p) { return p.scale.value.baseValue.x <= 0.0f || p.scale.value.baseValue.y <= 0.0f || p.scale.value.baseValue.z <= 0.0f; }),
		instances_.end());


	// SRT更新 & GPU転送データ作成
	for (auto& instance : instances_)
	{
		instance.scale.value.baseValue += instance.scale.velocity.baseValue * dt + 0.5f * instance.scale.acceleration.baseValue * dt * dt;
		instance.scale.velocity.baseValue += instance.scale.acceleration.baseValue * dt;

		Vector3 toCamera = cameraPos_ - Vector3(modelWorld_.m[3][0], modelWorld_.m[3][1], modelWorld_.m[3][2]);
		toCamera = toCamera.Normalize();

		float theta = std::atan2(toCamera.x, toCamera.z);
		Matrix4x4 billboardRot = Matrix::MakeRotationYMatrix(theta);

		const Matrix4x4 world = Matrix::MakeScaleMatrix(instance.scale.value.baseValue) * billboardRot * modelWorld_;

		particle_.pushInstance(world, instance.color);
	}
}


void BillboardScaleParticle::SetEnabled(bool isActive)
{
	isActive_ = isActive;
	if (isActive_)
	{
		emitTimer_ = config_.cfg.emitInterval;

	}
	else
	{
		emitTimer_ = 0.0f;
	}
}
