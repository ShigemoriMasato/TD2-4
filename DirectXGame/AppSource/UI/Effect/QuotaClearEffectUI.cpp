#include"QuotaClearEffectUI.h"
#include"FpsCount.h"
#include"Utility/Easing.h"
#include"RandomGenerator.h"

void QuotaClearEffectUI::Initialize(DrawData spriteDrawData, int texture) {

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

void QuotaClearEffectUI::Update() {

	// 生成
	if (isLoop_) {
		Create();
	}
	
	// 移動処理
	Move();
}

void QuotaClearEffectUI::Draw(Window* window, const Matrix4x4& vpMatrix) {

	// 描画
	instancingObject_->Draw(window, vpMatrix);
}

QuotaClearEffectUI::ParticleData QuotaClearEffectUI::MakeNewData(){
	ParticleData data;
	
	data.transform.position = pos_ + Vector3(RandomGenerator::Get(-range_.x, range_.x), RandomGenerator::Get(-range_.y, range_.y),0.0f);
	float scale = RandomGenerator::Get(8.0f, 30.0f);
	data.transform.scale = Vector3(scale, scale, 1.0f);
	data.startScale = scale;
	data.transform.rotate = Vector3(0.0f, 0.0f, 0.0f);
	data.transform.rotate.z = RandomGenerator::Get(0.0f, 6.4f);
	data.lifeTime = RandomGenerator::Get(0.5f, 1.0f);
	data.currentTime = 0.0f;
	data.textureHandle = texture_;
	data.color = { 1.0f,1.0f,1.0f,1.0f };
	return data;
}

void QuotaClearEffectUI::Create() {

	timer_ += FpsCount::deltaTime / coolTime_;

	if (timer_ >= 1.0f) {
		uint32_t spawnCount = 3;
		for (uint32_t i = 0; i < maxNum_ && spawnCount > 0; ++i) {
			if (particleDatas_[i].currentTime >= 1.0f ) {
				particleDatas_[i] = MakeNewData();
				spawnCount--;
			}
		}

		// タイマーをリセット
		timer_ = 0.0f;
	}
}

void QuotaClearEffectUI::Move() {
	// 現在の数をリセット
	index_ = 0;

	for (uint32_t i = 0; i < maxNum_; ++i) {
		ParticleData& data = particleDatas_[i];

		if (data.currentTime >= 1.0f) {
			continue;
		}
		data.currentTime += FpsCount::deltaTime / data.lifeTime;

		float scale = lerp(data.startScale, 0.0f, data.currentTime, EaseType::EaseInOutCubic);
		data.transform.scale.x = scale;
		data.transform.scale.y = scale;

		instancingObject_->transformDatas_[index_].color = data.color;
		instancingObject_->transformDatas_[index_].textureHandle = data.textureHandle;
		instancingObject_->transformDatas_[index_].worldMatrix = Matrix::MakeAffineMatrix(data.transform.scale, data.transform.rotate, data.transform.position);
		index_++;
	}

	instancingObject_->index_ = index_;
}