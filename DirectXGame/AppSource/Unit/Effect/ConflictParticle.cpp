#include"ConflictParticle.h"
#include"FpsCount.h"
#include"Utility/Easing.h"
#include"RandomGenerator.h"

void CollectParticle::Initialize(DrawData drawData) {

	instancingObject_ = std::make_unique<InstancingObject>();
	instancingObject_->Initialize(drawData);

	// ワールド行列の初期化
	particleDatas_.reserve(maxNum_);
	for (uint32_t i = 0; i < maxNum_; ++i) {
		particleDatas_.push_back(MakeNewData());
		particleDatas_.back().currentTime = 1.0f;
	}
}

void CollectParticle::Update() {

	Create();

	index_ = 0;

	// 移動処理
	Move();

	instancingObject_->index_ = index_;
}

void CollectParticle::Draw(Window* window, const Matrix4x4& vpMatrix) {

	// 描画
	instancingObject_->Draw(window, vpMatrix);
}

CollectParticle::ParticleData CollectParticle::MakeNewData() {
	ParticleData data;

	data.transform.position = pos_ + Vector3(RandomGenerator::Get(-200.0f, 0.0f), RandomGenerator::Get(-50.0f, 50.0f), 0.0f);
	float scale = RandomGenerator::Get(15.0f, 60.0f);
	data.transform.scale = Vector3(scale, scale, 1.0f);
	data.startScale = scale;
	data.transform.rotate = Vector3(0.0f, 0.0f, 0.0f);
	data.transform.rotate.z = RandomGenerator::Get(0.0f, 6.4f);
	data.lifeTime = RandomGenerator::Get(1.0f, 1.5f);
	data.currentTime = 0.0f;
	data.textureHandle = texture_;
	data.speed = RandomGenerator::Get(750.0f, 2800.0f);
	data.rotSpeed = 12.0f;
	data.color = { 1.0f,1.0f,1.0f,1.0f };
	return data;
}

void CollectParticle::Create() {

	timer_ += FpsCount::deltaTime / coolTime_;

	if (timer_ >= 1.0f) {
		uint32_t spawnCount = 25;
		for (uint32_t i = 0; i < maxNum_ && spawnCount > 0; ++i) {
			if (particleDatas_[i].currentTime >= 1.0f) {
				particleDatas_[i] = MakeNewData();
				spawnCount--;
			}
		}

		// タイマーをリセット
		timer_ = 0.0f;
	}
}

void CollectParticle::Move() {
	// 現在の数をリセット
	for (uint32_t i = 0; i < maxNum_; ++i) {
		ParticleData& data = particleDatas_[i];

		if (data.currentTime >= 1.0f) {
			continue;
		}
		data.currentTime += FpsCount::deltaTime / data.lifeTime;

		// 移動
		data.speed -= 4500.0f * FpsCount::deltaTime;
		if (data.speed <= 0.0f) {
			data.speed = 0.0;
		}
		data.transform.position.x -= data.speed * FpsCount::deltaTime;

		float scale = lerp(data.startScale, 0.0f, data.currentTime, EaseType::EaseInOutCubic);
		data.transform.scale.x = scale;
		data.transform.scale.y = scale;

		data.transform.rotate.z += data.rotSpeed * FpsCount::deltaTime;

		instancingObject_->transformDatas_[index_].color = data.color;
		instancingObject_->transformDatas_[index_].textureHandle = data.textureHandle;
		instancingObject_->transformDatas_[index_].worldMatrix = Matrix::MakeAffineMatrix(data.transform.scale, data.transform.rotate, data.transform.position);
		index_++;
	}
}