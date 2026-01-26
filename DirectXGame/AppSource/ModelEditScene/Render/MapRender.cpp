#include "MapRender.h"

void MapRender::Initialize(const DrawData& box) {
	render_ = std::make_unique<RenderObject>();
	render_->Initialize();
	render_->CreateSRV(sizeof(VSData), maxSize_ * maxSize_, ShaderType::VERTEX_SHADER, "WVP");
	render_->CreateSRV(sizeof(Vector4), maxSize_ * maxSize_, ShaderType::PIXEL_SHADER, "Color");
	render_->CreateSRV(sizeof(int), maxSize_ * maxSize_, ShaderType::PIXEL_SHADER, "TextureIndex");
	render_->SetDrawData(box);

	render_->psoConfig_.vs = "Simples.VS.hlsl";
	render_->psoConfig_.ps = "MapRender.PS.hlsl";

	worldMatrices_.resize(maxSize_ * maxSize_, Matrix4x4::Identity());
	float yOffset = 0.05f;
	for (int i = 0; i < maxSize_; ++i) {
		for (int j = 0; j < maxSize_; ++j) {
			int index = i * maxSize_ + j;
			worldMatrices_[index] = Matrix::MakeAffineMatrix({ 1.0f, 1.0f, 1.0f }, {}, { float(j) + 0.5f, yOffset, float(i) + 0.5f });
		}
	}
}

void MapRender::Update() {
}

void MapRender::Draw(const Matrix4x4& vpMatrix, Window* window) {
	//成型
	std::fill(colors_.begin(), colors_.end(), Vector4(1.0f, 1.0f, 1.0f, alpha_));
	for (auto& data : vsData_) {
		data.vp = vpMatrix;
	}

	//コピー
	render_->CopyBufferData(0, vsData_.data(), sizeof(VSData) * vsData_.size());
	render_->CopyBufferData(1, colors_.data(), sizeof(Vector4) * colors_.size());
	render_->CopyBufferData(2, textureIndices_.data(), sizeof(int) * textureIndices_.size());
	render_->instanceNum_ = static_cast<uint32_t>(vsData_.size());

	//描画
	render_->Draw(window);
}

void MapRender::SetConfig(int width, int height, const std::vector<std::vector<int>>& textureIndices) {
	if (textureIndices.size() != static_cast<size_t>(width * height)) {
		return;
	}

	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			int index = i * width + j;
			vsData_[index].world = worldMatrices_[index];
			textureIndices_[index] = textureIndices[i][j];
		}
	}
}
