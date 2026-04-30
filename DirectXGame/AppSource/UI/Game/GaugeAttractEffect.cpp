#include "GaugeAttractEffect.h"
#include <algorithm>

#ifdef USE_IMGUI
#include <imgui/imgui.h>
#endif

using namespace SHEngine;

void GaugeAttractEffect::Initialize(
    const Vector3& start, const Vector3& end, SHEngine::DrawDataManager* drawDataManager, SHEngine::ModelManager* modelManager, SHEngine::TextureManager* textureManager, Vector3 control1,
    Vector3 control2) {
	startPos_ = start;
	endPos_ = end;

	// 制御点の設定
	Vector3 dir = end - start;
	control1_ = start + control1 + offset1_;
	control2_ = start + control2 + offset2_;

	transform_.scale = {108.0f, 64.0f, 1.0f};
	transform_.rotate = {0.0f, 0.0f, 0.0f};
	explosionScale_ = transform_.scale;

	int modelHandle = modelManager->LoadModel("Assets/.EngineResource/Model/plane");
	auto modelData = modelManager->GetNodeModelData(modelHandle);
	DrawData data = drawDataManager->GetDrawData(modelData.drawDataIndex);
	render_ = std::make_unique<RenderObject>();
	render_->Initialize();
	render_->psoConfig_.vs = "Simples.VS.hlsl";
	render_->psoConfig_.ps = "TexColors.PS.hlsl";
	render_->SetDrawData(data);
	render_->CreateSRV(sizeof(Matrix4x4), kTrailCount, ShaderType::VERTEX_SHADER, "WVP");
	render_->CreateSRV(sizeof(Vector4), kTrailCount, ShaderType::PIXEL_SHADER, "Color");
	render_->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");
	render_->SetUseTexture(true);
	render_->instanceNum_ = kTrailCount;
	textureIndex_ = textureManager->LoadTexture("AttractEffect.png");

	explosionRender_ = std::make_unique<RenderObject>();
	explosionRender_->Initialize();
	explosionRender_->psoConfig_.vs = "Simples.VS.hlsl";
	explosionRender_->psoConfig_.ps = "Game/AfterImage.PS.hlsl";
	explosionRender_->SetDrawData(data);
	explosionRender_->CreateSRV(sizeof(Matrix4x4), 2, ShaderType::VERTEX_SHADER, "WVP");
	explosionRender_->CreateSRV(sizeof(Vector4), 2, ShaderType::PIXEL_SHADER, "Color");
	explosionRender_->CreateSRV(sizeof(int), 2, ShaderType::PIXEL_SHADER, "TextureIndex");
	explosionRender_->SetUseTexture(true);
	explosionRender_->instanceNum_ = 2;
	ringTexture_ = textureManager->LoadTexture("Ring.png");
	crossTexture_ = textureManager->LoadTexture("Cross.png");
}

void GaugeAttractEffect::Update(Matrix4x4 vpMatrix, float deltaTime) {
	if (isFinished_)
		return;

	if (!isMovingFinished_) {
		time_ += deltaTime / duration_;
		if (time_ >= 1.0f) {
			time_ = 1.0f;
			isMovingFinished_ = true;
			scaleAnim_.Start(explosionScale_, explosionScale_ * 4.0f, 0.5f, EaseType::EaseOutCubic);
			alphaAnim_.Start(1.0f, 0.0f, 0.5f, EaseType::EaseOutCubic);
		}

		std::vector<Matrix4x4> wvpMatrices(kTrailCount);
		std::vector<Vector4> colors(kTrailCount);

		for (int i = 0; i < kTrailCount; ++i) {
			float t = std::max(0.0f, time_ - (i * trailDelay_));
			Vector3 pos = EvaluateBezier(t);
			Vector3 scale = transform_.scale;
			scale.x *= std::max(0.0f, 1.0f - (i * 0.15f));
			scale.y *= std::max(0.0f, 1.0f - (i * 0.15f));

			Matrix4x4 wvpMatrix = Matrix::MakeAffineMatrix(scale, transform_.rotate, pos);
			wvpMatrices[i] = wvpMatrix * vpMatrix;

			float alpha = std::max(0.0f, 1.0f - (static_cast<float>(i) / kTrailCount));
			colors[i] = {1.0f, 1.0f, 1.0f, alpha};
		}

		render_->CopyBufferData(0, wvpMatrices.data(), sizeof(Matrix4x4) * kTrailCount);
		render_->CopyBufferData(1, colors.data(), sizeof(Vector4) * kTrailCount);
		render_->CopyBufferData(2, &textureIndex_, sizeof(int));
	} else {
		std::vector<Matrix4x4> wvpMatrices(2);
		std::vector<Vector4> colors(2);
		std::vector<int> textures(2);

		textures[0] = ringTexture_;
		textures[1] = crossTexture_;

		float alpha;
		bool scaleActive = scaleAnim_.Update(deltaTime, explosionScale_);
		bool opacityActive = alphaAnim_.Update(deltaTime, alpha);

		// 両方のアニメーションが終わったら削除フラグを立てる
		if (!scaleActive && !opacityActive) {
			isFinished_ = true;
		}

		for (int i = 0; i < 2; ++i) {
			// 行列と色の更新
			Matrix4x4 world = Matrix::MakeAffineMatrix(explosionScale_, {0, 0, 0}, endPos_);
			Matrix4x4 wvp = world * vpMatrix;
			Vector4 color = {1.0f, 1.0f, 1.0f, alpha};

			wvpMatrices[i] = world * vpMatrix;
			colors[i] = color;

			explosionRender_->CopyBufferData(0, wvpMatrices.data(), sizeof(Matrix4x4) * 2);
			explosionRender_->CopyBufferData(1, colors.data(), sizeof(Vector4) * 2);
			explosionRender_->CopyBufferData(2, textures.data(), sizeof(int) * 2);
		}
	}
}

void GaugeAttractEffect::Draw(CmdObj* cmdObj) {
	if (isFinished_)
		return;

	if (!isMovingFinished_) {
		render_->Draw(cmdObj);
	} else {
		explosionRender_->Draw(cmdObj);
	}
}

Vector3 GaugeAttractEffect::EvaluateBezier(float t) const {
	float easedT = lerp<float>(0.0f, 1.0f, t, easeType_);
	float u = 1.0f - easedT;
	float uu = u * u;
	float uuu = uu * u;
	float tt = easedT * easedT;
	float ttt = tt * easedT;

	Vector3 p = uuu * startPos_;
	p += 3.0f * uu * easedT * control1_;
	p += 3.0f * u * tt * control2_;
	p += ttt * endPos_;

	return p;
}