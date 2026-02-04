#include"ConfettiEffectUI.h"
#include"FpsCount.h"
#include"Utility/Easing.h"
#include"RandomGenerator.h"

void ConfettiEffectUI::Initialize(DrawData spriteDrawData, int texture) {

	instancingObject_ = std::make_unique<InstancingObject>();
	instancingObject_->Initialize(spriteDrawData);

	// ワールド行列の初期化
	particleDatas_.reserve(maxNum_);
	for (uint32_t i = 0; i < maxNum_; ++i) {
		particleDatas_.push_back(MakeNewData());
		particleDatas_.back().currentTime = 1.0f;
	}

	texture_ = texture;
}

void ConfettiEffectUI::Update() {

	// 生成
	if (isLoop_) {
		Create();
	}

	// 移動処理
	Move();
}

void ConfettiEffectUI::Draw(Window* window, const Matrix4x4& vpMatrix) {

	// 描画
	instancingObject_->Draw(window, vpMatrix);
}

ConfettiEffectUI::ParticleData ConfettiEffectUI::MakeNewData() {
	ParticleData data;

	// 発生位置s
	data.transform.position = pos_ + Vector3(RandomGenerator::Get(-range_.x, range_.x), RandomGenerator::Get(-range_.y, range_.y), 0.0f);

	// スケールと初期回転
	float scale = RandomGenerator::Get(10.0f, 40.0f);
	data.transform.scale = Vector3(scale, scale, 1.0f);
	data.startScale = scale;
	data.transform.rotate = Vector3(0.0f, 0.0f, RandomGenerator::Get(0.0f, 6.28f));
	// 回転しながら飛ぶように回転速度をランダム設定
	data.rotateVelocity.x = RandomGenerator::Get(-3.0f, 3.0f);
	data.rotateVelocity.y = RandomGenerator::Get(-3.0f, 3.0f);
	data.rotateVelocity.z = RandomGenerator::Get(-3.0f, 3.0f);
	// 寿命
	data.lifeTime = RandomGenerator::Get(1.0f, 2.0f);
	data.currentTime = 0.0f;
	data.textureHandle = texture_;
	
	data.color = {
		RandomGenerator::Get(0.2f, 1.0f),
		RandomGenerator::Get(0.2f, 1.0f),
		RandomGenerator::Get(0.2f, 1.0f),
		1.0f
	};

	float velocityX = 0.0f;
	float velocityY = 0.0f;
	float speedX = RandomGenerator::Get(200.0f, 500.0f);
	float speedY = RandomGenerator::Get(600.0f, 1200.0f);
	
	if (pos_.x < 640.0f) {
		// 左から右へ
		velocityX = speedX;
	} else {
		// 右から左へ
		velocityX = -speedX;
	}
	// 上方向へ打ち上げ
	velocityY = speedY;

	data.velocity = Vector3(velocityX, -velocityY, 0.0f);

	return data;
}

void ConfettiEffectUI::Create() {

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

void ConfettiEffectUI::Move() {

	// 現在の数をリセット
	index_ = 0;

	float dt = FpsCount::deltaTime; // デルタタイムを変数に受けておく

	for (uint32_t i = 0; i < maxNum_; ++i) {
		ParticleData& data = particleDatas_[i];

		if (data.currentTime >= 1.0f) {
			continue;
		}

		// 時間経過
		data.currentTime += dt / data.lifeTime;

		data.velocity.y += 980.0f * dt;

		// 速度を位置に加算
		data.transform.position += data.velocity * dt;

		// くるくると回転させる
		data.transform.rotate += data.rotateVelocity * dt;

		float scale = lerp(data.startScale, 0.0f, data.currentTime, EaseType::EaseInQuad);
		data.transform.scale.x = scale;
		data.transform.scale.y = scale;

		// データのセット
		instancingObject_->transformDatas_[index_].color = data.color;
		instancingObject_->transformDatas_[index_].textureHandle = data.textureHandle;
		instancingObject_->transformDatas_[index_].worldMatrix = Matrix::MakeAffineMatrix(data.transform.scale, data.transform.rotate, data.transform.position);
		index_++;
	}

	instancingObject_->index_ = index_;
}