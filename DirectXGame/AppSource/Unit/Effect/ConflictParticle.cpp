#include"ConflictParticle.h"
#include"FpsCount.h"
#include"Utility/Easing.h"
#include"RandomGenerator.h"

void ConflictParticle::Initialize(DrawData drawData) {

	instancingObject_ = std::make_unique<InstancingObject>();
	instancingObject_->maxNum_ = conflictMaxNum_ + smokeMaxNum_ + dirtMaxNum_;
	instancingObject_->Initialize(drawData);

	// ワールド行列の初期化
	particleDatas_.reserve(conflictMaxNum_);
	for (uint32_t i = 0; i < conflictMaxNum_; ++i) {
		particleDatas_.push_back(MakeNewData({0.0f,0.0f,0.0f}));
		particleDatas_.back().currentTime = 1.0f;
	}

	// ワールド行列の初期化
	smokeDatas_.reserve(smokeMaxNum_);
	for (uint32_t i = 0; i < smokeMaxNum_; ++i) {
		smokeDatas_.push_back(MakeNewData({ 0.0f,0.0f,0.0f }));
		smokeDatas_.back().currentTime = 1.0f;
	}

	// ワールド行列の初期化
	dirtDatas_.reserve(dirtMaxNum_);
	for (uint32_t i = 0; i < dirtMaxNum_; ++i) {
		dirtDatas_.push_back(MakeNewData({ 0.0f,0.0f,0.0f }));
		dirtDatas_.back().currentTime = 1.0f;
	}
}

void ConflictParticle::Update() {

	index_ = 0;

	// 移動処理
	Move();

	// 煙パーティクル
	SmokeMove();

	// 泥パーティクル
	DirtMove();

	instancingObject_->index_ = index_;
}

void ConflictParticle::Draw(Window* window, const Matrix4x4& vpMatrix) {

	// 描画
	instancingObject_->Draw(window, vpMatrix);
}

ConflictParticle::ParticleData ConflictParticle::MakeNewData(const Vector3& pos) {
	ParticleData data;

	data.transform.position = pos + Vector3(RandomGenerator::Get(-1.0f, 1.0f), RandomGenerator::Get(0.0f, 1.0f), RandomGenerator::Get(-1.0f, 1.0f));
	float scale = RandomGenerator::Get(0.2f, 0.8f);
	data.transform.scale = Vector3(scale, scale, scale);
	data.startScale = scale;
	data.transform.rotate = RandomGenerator::GetVector3(0.0f,6.4f);
	data.lifeTime = RandomGenerator::Get(0.8f, 1.0f);
	data.currentTime = 0.0f;
	data.textureHandle = 0;
	data.speed = RandomGenerator::Get(1.0f, 10.0f);
	data.rotSpeed = RandomGenerator::GetVector3(-6.4f, 6.4f);
	data.color = { 1.0f,1.0f,1.0f,1.0f };

	Vector3 dir = data.transform.position - pos;
	data.velocity = dir.Normalize() * data.speed;
	return data;
}

void ConflictParticle::Create() {

	timer_ += FpsCount::deltaTime / coolTime_;

	if (timer_ >= 1.0f) {
		uint32_t spawnCount = 25;
		for (uint32_t i = 0; i < conflictMaxNum_ && spawnCount > 0; ++i) {
			if (particleDatas_[i].currentTime >= 1.0f) {
				particleDatas_[i] = MakeNewData({0.0f,0.0f,0.0f});
				spawnCount--;
			}
		}

		// タイマーをリセット
		timer_ = 0.0f;
	}
}

void ConflictParticle::Move() {
	// 現在の数をリセット
	for (uint32_t i = 0; i < conflictMaxNum_; ++i) {
		ParticleData& data = particleDatas_[i];

		if (data.currentTime >= 1.0f) {
			continue;
		}
		data.currentTime += FpsCount::deltaTime / data.lifeTime;

		// 移動
		data.transform.position += data.velocity * FpsCount::deltaTime;

		float scale = lerp(data.startScale, 0.0f, data.currentTime, EaseType::EaseInOutCubic);
		data.transform.scale = { scale ,scale ,scale };

		data.transform.rotate += data.rotSpeed * FpsCount::deltaTime;

		instancingObject_->transformDatas_[index_].color = data.color;
		instancingObject_->transformDatas_[index_].textureHandle = data.textureHandle;
		instancingObject_->transformDatas_[index_].worldMatrix = Matrix::MakeAffineMatrix(data.transform.scale, data.transform.rotate, data.transform.position);
		index_++;
	}
}

void ConflictParticle::AddConflict(const Vector3& pos) {

	uint32_t spawnCount = 5;
	for (uint32_t i = 0; i < conflictMaxNum_ && spawnCount > 0; ++i) {
		if (particleDatas_[i].currentTime >= 1.0f) {
			particleDatas_[i] = MakeNewData(pos);
			spawnCount--;
		}
	}
}

ConflictParticle::ParticleData ConflictParticle::MakeNewSmokeData(const Vector3& pos) {
	ParticleData data;

	data.transform.position = pos + Vector3(RandomGenerator::Get(-0.4f, 0.4f), RandomGenerator::Get(0.4f, 1.0f), RandomGenerator::Get(-0.4f, 0.4f));
	float scale = RandomGenerator::Get(0.2f, 0.5f);
	data.transform.scale = Vector3(scale, scale, scale);
	data.startScale = scale;
	data.transform.rotate = RandomGenerator::GetVector3(0.0f, 6.4f);
	data.lifeTime = RandomGenerator::Get(1.5f, 3.0f);
	data.currentTime = 0.0f;
	data.textureHandle = 0;
	data.speed = RandomGenerator::Get(0.8f, 1.5f);
	data.rotSpeed = RandomGenerator::GetVector3(-6.4f, 6.4f);
	data.color = { 1.0f,1.0f,1.0f,1.0f };

	Vector3 dir = data.transform.position - pos;
	dir.y *= -1.0f;
	data.velocity = -dir.Normalize() * data.speed;
	return data;
}

void ConflictParticle::SmokeMove() {
	// 現在の数をリセット
	for (uint32_t i = 0; i < smokeMaxNum_; ++i) {
		ParticleData& data = smokeDatas_[i];

		if (data.currentTime >= 1.0f) {
			continue;
		}
		data.currentTime += FpsCount::deltaTime / data.lifeTime;

		// 移動
		data.transform.position += data.velocity * FpsCount::deltaTime;

		float scale = lerp(data.startScale, 0.0f, data.currentTime, EaseType::EaseInOutCubic);
		data.transform.scale = { scale ,scale ,scale };

		data.transform.rotate += data.rotSpeed * FpsCount::deltaTime;

		instancingObject_->transformDatas_[index_].color = data.color;
		instancingObject_->transformDatas_[index_].textureHandle = data.textureHandle;
		instancingObject_->transformDatas_[index_].worldMatrix = Matrix::MakeAffineMatrix(data.transform.scale, data.transform.rotate, data.transform.position);
		index_++;
	}
}

void ConflictParticle::AddSmoke(const Vector3& pos) {

	uint32_t spawnCount = 5;
	for (uint32_t i = 0; i < smokeMaxNum_ && spawnCount > 0; ++i) {
		if (smokeDatas_[i].currentTime >= 1.0f) {
			smokeDatas_[i] = MakeNewSmokeData(pos);
			spawnCount--;
		}
	}
}

ConflictParticle::ParticleData ConflictParticle::MakeNewDirtData(const Vector3& pos) {
	ParticleData data;

	data.transform.position = pos + Vector3(RandomGenerator::Get(-0.4f, 0.4f), RandomGenerator::Get(0.4f, 1.0f), RandomGenerator::Get(-0.4f, 0.4f));
	float scale = RandomGenerator::Get(0.2f, 0.5f);
	data.transform.scale = Vector3(scale, scale, scale);
	data.startScale = scale;
	data.transform.rotate = RandomGenerator::GetVector3(0.0f, 6.4f);
	data.lifeTime = RandomGenerator::Get(1.5f, 3.0f);
	data.currentTime = 0.0f;
	data.textureHandle = 0;
	data.speed = RandomGenerator::Get(0.8f, 1.5f);
	data.rotSpeed = RandomGenerator::GetVector3(-6.4f, 6.4f);
	data.color = { 1.0f,1.0f,1.0f,1.0f };

	Vector3 dir = data.transform.position - pos;
	dir.y *= -1.0f;
	data.velocity = -dir.Normalize() * data.speed;
	return data;
}

void ConflictParticle::DirtMove() {
	// 現在の数をリセット
	for (uint32_t i = 0; i < dirtMaxNum_; ++i) {
		ParticleData& data = dirtDatas_[i];

		if (data.currentTime >= 1.0f) {
			continue;
		}
		data.currentTime += FpsCount::deltaTime / data.lifeTime;

		// 移動
		data.transform.position += data.velocity * FpsCount::deltaTime;

		float scale = lerp(data.startScale, 0.0f, data.currentTime, EaseType::EaseInOutCubic);
		data.transform.scale = { scale ,scale ,scale };

		data.transform.rotate += data.rotSpeed * FpsCount::deltaTime;

		instancingObject_->transformDatas_[index_].color = data.color;
		instancingObject_->transformDatas_[index_].textureHandle = data.textureHandle;
		instancingObject_->transformDatas_[index_].worldMatrix = Matrix::MakeAffineMatrix(data.transform.scale, data.transform.rotate, data.transform.position);
		index_++;
	}
}

void ConflictParticle::AddDirt(const Vector3& pos) {

	uint32_t spawnCount = 5;
	for (uint32_t i = 0; i < dirtMaxNum_ && spawnCount > 0; ++i) {
		if (dirtDatas_[i].currentTime >= 1.0f) {
			dirtDatas_[i] = MakeNewSmokeData(pos);
			spawnCount--;
		}
	}
}