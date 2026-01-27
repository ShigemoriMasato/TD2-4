#include "MapRender.h"
#include "../Editor/Texture/MapTextureEditor.h"
#include <numbers>

void MapRender::Initialize(const DrawData& box) {
	render_ = std::make_unique<RenderObject>();
	render_->Initialize();
	render_->CreateSRV(sizeof(VSData), maxSize_ * maxSize_, ShaderType::VERTEX_SHADER, "WVP");
	render_->CreateSRV(sizeof(Vector4), maxSize_ * maxSize_, ShaderType::PIXEL_SHADER, "Color");
	render_->CreateSRV(sizeof(int), maxSize_ * maxSize_, ShaderType::PIXEL_SHADER, "TextureIndex");
	render_->SetUseTexture(true);
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

	colors_.resize(maxSize_ * maxSize_, Vector4(1.0f, 1.0f, 1.0f, alpha_));
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

void MapRender::SetConfig(const std::vector<std::vector<ChipData>>& textureIndices) {
	vsData_.clear();
	textureIndices_.clear();
	for (size_t i = 0; i < textureIndices.size(); ++i) {
		for (size_t j = 0; j < textureIndices[i].size(); ++j) {
			VSData data;
			Matrix4x4 rotateMatrix;
			rotateMatrix = Matrix::MakeRotationYMatrix(static_cast<float>((static_cast<int>(textureIndices[i][j].direction) + 1) * (std::numbers::pi_v<float> / 2.0f)));
			data.world = rotateMatrix * worldMatrices_[i * maxSize_ + j];
			vsData_.push_back(data);
			textureIndices_.push_back(textureIndices[i][j].textureIndex);
		}
	}
}
