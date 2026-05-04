#include "BillboardColorParticle.h"
#include <Utility/MatrixFactory.h>

void BillboardColorParticle::Initialize(SHEngine::TextureManager* textureManager, SHEngine::ModelManager* modelManager)
{
	particle_.Initialize(textureManager, modelManager);
}

void BillboardColorParticle::SetConfig(const BillboardColorConfig& config)
{
	config_ = config;
	particle_.SetModel(config.cfg.modelPath);
	particle_.SetTexture(config.cfg.texturePath);
}


void BillboardColorParticle::Update(float dt)
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
			ins.color = config_.color;
			ins.scale = config_.scale;

			instances_.push_back(ins);
		}
	}

	emitTimer_ += dt;


	// SRT更新 & 年齢更新
	for (auto& instance : instances_)
	{
		instance.age += dt;
		instance.scale.value.baseValue += instance.scale.velocity.baseValue * dt + 0.5f * instance.scale.acceleration.baseValue * dt * dt;
		instance.scale.velocity.baseValue += instance.scale.acceleration.baseValue * dt;
		instance.color.value.baseValue += instance.color.velocity.baseValue * dt + 0.5f * instance.color.acceleration.baseValue * dt * dt;
		instance.color.velocity.baseValue += instance.color.acceleration.baseValue * dt;
	}

	// 寿命切れ削除
	instances_.erase(
		std::remove_if(instances_.begin(), instances_.end(),
			[this](const ParticleInstance& p) { return p.age >= config_.cfg.lifeTime; }),
		instances_.end());
	instances_.erase(
		std::remove_if(instances_.begin(), instances_.end(),
			[this](const ParticleInstance& p) { return p.color.value.baseValue.w < 0.0f; }),
		instances_.end());

	Vector3 myPos = Vector3(modelWorld_.m[3][0], modelWorld_.m[3][1], modelWorld_.m[3][2]);
	Vector3 cameraPos = cameraPos_;
	Vector3 toCameraDir = cameraPos - myPos;
	Vector3 normDir = toCameraDir.Normalize();
	float pitch = std::asinf(-normDir.y); // -sin(pitch) = y 成分
	float yaw = std::atan2f(normDir.x, normDir.z); // sin(yaw) = x 成分, cos(yaw) = z 成分
	Vector3 rotate(pitch, yaw, 0.0f); // roll はここでは未使用

	// SRT更新 & GPU転送データ作成
	for (auto& instance : instances_)
	{
		const Matrix4x4 world = Matrix::MakeAffineMatrix(instance.scale.value.baseValue, rotate, myPos);

		instance.color.value.baseValue.x = std::clamp(instance.color.value.baseValue.x, 0.0f, 1.0f);
		instance.color.value.baseValue.y = std::clamp(instance.color.value.baseValue.y, 0.0f, 1.0f);
		instance.color.value.baseValue.z = std::clamp(instance.color.value.baseValue.z, 0.0f, 1.0f);
		instance.color.value.baseValue.w = std::clamp(instance.color.value.baseValue.w, 0.0f, 1.0f);

		particle_.pushInstance(world, instance.color.value.baseValue);
	}
}


void BillboardColorParticle::SetEnabled(bool isActive)
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
