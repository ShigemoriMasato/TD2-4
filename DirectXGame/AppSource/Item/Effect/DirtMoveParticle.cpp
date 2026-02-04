#include"DirtMoveParticle.h"
#include"FpsCount.h"
#include"Utility/Easing.h"
#include"RandomGenerator.h"
#include <Common/DebugParam/GameParamEditor.h>

void DirtMoveParticle::Initialize(DrawData drawData) {

	instancingObject_ = std::make_unique<InstancingObject>();
	instancingObject_->maxNum_ = maxNum_;
	instancingObject_->Initialize(drawData);

	// ワールド行列の初期化
	particleDatas_.reserve(maxNum_);
	for (uint32_t i = 0; i < maxNum_; ++i) {
		particleDatas_.push_back(MakeNewData({ 0.0f,0.0f,0.0f }));
		particleDatas_.back().currentTime = 1.0f;
	}

//#ifdef USE_IMGUI
//	RegisterDebugParam();
//#endif
//	ApplyDebugParam();
}

void DirtMoveParticle::Update() {
//#ifdef USE_IMGUI
//	ApplyDebugParam();
//#endif

	// 移動処理
	Move();
}

void DirtMoveParticle::Draw(Window* window, const Matrix4x4& vpMatrix) {

	// 描画
	instancingObject_->Draw(window, vpMatrix);
}

DirtMoveParticle::ParticleData DirtMoveParticle::MakeNewData(const Vector3& pos) {
	ParticleData data;

	data.transform.position = pos + Vector3(RandomGenerator::Get(-0.4f, 0.4f), RandomGenerator::Get(0.2f, 0.5f), RandomGenerator::Get(-0.4f, 0.4f));
	float scale = RandomGenerator::Get(0.08f, 0.2f);
	data.transform.scale = Vector3(scale, scale, scale);
	data.startScale = scale;
	data.transform.rotate = RandomGenerator::GetVector3(0.0f, 6.4f);
	data.lifeTime = RandomGenerator::Get(1.0f, 1.5f);
	data.currentTime = 0.0f;
	data.textureHandle = 0;
	data.speed = RandomGenerator::Get(0.5f, 1.0f);
	data.rotSpeed = 12.0f;

	int i = RandomGenerator::Get(int(0), int(2));
	if (i == 0) {
		data.color = { 0.2f,0.2f,0.2f,1.0f };
	} else if (i == 1) {
		data.color = { 0.1f,0.1f,0.1f,1.0f };
	} else {
		data.color = { 0.0f,0.0f,0.0f,1.0f };
	}

	data.velocity = Vector3(0,1,0) * data.speed;
	return data;
}

void DirtMoveParticle::Move() {
	// 現在の数をリセット
	index_ = 0;

	for (uint32_t i = 0; i < maxNum_; ++i) {
		ParticleData& data = particleDatas_[i];

		if (data.currentTime >= 1.0f) {
			continue;
		}
		data.currentTime += FpsCount::deltaTime / data.lifeTime;

		data.velocity.y -= 5.0f * FpsCount::deltaTime;

		// 移動
		data.transform.position += data.velocity * FpsCount::deltaTime;

		data.color.w = lerp(1.0f, 0.0f, data.currentTime, EaseType::EaseInOutCubic);

		instancingObject_->transformDatas_[index_].color = data.color;
		instancingObject_->transformDatas_[index_].textureHandle = data.textureHandle;
		instancingObject_->transformDatas_[index_].worldMatrix = Matrix::MakeAffineMatrix(data.transform.scale, data.transform.rotate, data.transform.position);
		index_++;
	}

	instancingObject_->index_ = index_;
}

void DirtMoveParticle::AddParticle(const Vector3& pos) {
	uint32_t spawnCount = 2;
	uint32_t checkedCount = 0;
	while (spawnCount > 0 && checkedCount < maxNum_) {
		ParticleData& data = particleDatas_[dirtEmitIndex_];

		if (data.currentTime >= 1.0f) {
			data = MakeNewData(pos);
			spawnCount--;
		}
		dirtEmitIndex_ = (dirtEmitIndex_ + 1) % maxNum_;
		checkedCount++;
	}
}

void DirtMoveParticle::RegisterDebugParam() {
	GameParamEditor::GetInstance()->AddItem("OreItemFragmentParticle", "minScale", minScale_);
	GameParamEditor::GetInstance()->AddItem("OreItemFragmentParticle", "maxScale", maxScale_);
}

void DirtMoveParticle::ApplyDebugParam() {
	minScale_ = GameParamEditor::GetInstance()->GetValue<float>("OreItemFragmentParticle", "minScale");
	maxScale_ = GameParamEditor::GetInstance()->GetValue<float>("OreItemFragmentParticle", "maxScale");
}