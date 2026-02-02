#include"OreFragmentParticle.h"
#include"FpsCount.h"
#include"Utility/Easing.h"
#include"RandomGenerator.h"

void OreFragmentParticle::Initialize(DrawData drawData) {

	instancingObject_ = std::make_unique<InstancingObject>();
	instancingObject_->Initialize(drawData);
	instancingObject_->maxNum_ = maxNum_;

	// ワールド行列の初期化
	particleDatas_.reserve(maxNum_);
	for (uint32_t i = 0; i < maxNum_; ++i) {
		particleDatas_.push_back(MakeNewData({0.0f,0.0f,0.0f}));
		particleDatas_.back().currentTime = 1.0f;
	}
}

void OreFragmentParticle::Update() {

	// 移動処理
	Move();
}

void OreFragmentParticle::Draw(Window* window, const Matrix4x4& vpMatrix) {

	// 描画
	instancingObject_->Draw(window, vpMatrix);
}

OreFragmentParticle::ParticleData OreFragmentParticle::MakeNewData(const Vector3& pos) {
	ParticleData data;

	data.transform.position = pos + Vector3(RandomGenerator::Get(-0.4f, 0.4f), RandomGenerator::Get(0.2f, 0.5f), RandomGenerator::Get(-0.4f, 0.4f));
	float scale = RandomGenerator::Get(0.08f, 0.2f);
	data.transform.scale = Vector3(scale, scale, scale);
	data.startScale = scale;
	data.transform.rotate = RandomGenerator::GetVector3(0.0f, 6.4f);
	data.lifeTime = RandomGenerator::Get(1.0f, 1.5f);
	data.currentTime = 0.0f;
	data.textureHandle = 0;
	data.speed = RandomGenerator::Get(1.0f, 5.0f);
	data.rotSpeed = 12.0f;
	data.color = { 0.9f,0.9f,0.0f,1.0f };

	Vector3 dir = data.transform.position - pos;
	data.velocity = dir.Normalize() * data.speed;
	return data;
}

void OreFragmentParticle::Move() {
	// 現在の数をリセット
	index_ = 0;

	for (uint32_t i = 0; i < maxNum_; ++i) {
		ParticleData& data = particleDatas_[i];

		if (data.currentTime >= 1.0f) {
			continue;
		}
		data.currentTime += FpsCount::deltaTime / data.lifeTime;

		data.velocity.y -= 20.0f * FpsCount::deltaTime;

		// 移動
		data.transform.position += data.velocity * FpsCount::deltaTime;

		if (data.transform.position.y <= 0.0f) {
			data.velocity.y = 0.8f * -data.velocity.y;
		}

		//float scale = lerp(data.startScale, 0.0f, data.currentTime, EaseType::EaseInOutCubic);
		//data.transform.scale = Vector3(scale, scale, scale);

		data.color.w = lerp(1.0f, 0.0f, data.currentTime, EaseType::EaseInOutCubic);

		instancingObject_->transformDatas_[index_].color = data.color;
		instancingObject_->transformDatas_[index_].textureHandle = data.textureHandle;
		instancingObject_->transformDatas_[index_].worldMatrix = Matrix::MakeAffineMatrix(data.transform.scale, data.transform.rotate, data.transform.position);
		index_++;
	}

	instancingObject_->index_ = index_;
}

void OreFragmentParticle::AddParticle(const Vector3& pos) {
	uint32_t spawnCount = 5;
	for (uint32_t i = 0; i < maxNum_ && spawnCount > 0; ++i) {
		if (particleDatas_[i].currentTime >= 1.0f) {
			particleDatas_[i] = MakeNewData(pos);
			spawnCount--;
		}
	}
}