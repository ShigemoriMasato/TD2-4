#include "DebugMCRender.h"
#include <Utility/Easing.h>
#include <algorithm>

void DebugMCRender::Initialize(const DrawData& drawData) {
	render_ = std::make_unique<RenderObject>();
	render_->Initialize();
	render_->CreateSRV(sizeof(MatrixData), maxSize_ * maxSize_, ShaderType::VERTEX_SHADER, "WVP");
	render_->CreateSRV(sizeof(Vector4), maxSize_ * maxSize_, ShaderType::PIXEL_SHADER, "Color");
	render_->SetDrawData(drawData);
	render_->psoConfig_.vs = "Simples.VS.hlsl";
	render_->psoConfig_.ps = "DebugBlocks.PS.hlsl";

	//worldMatrixの初期化
	forGPUData_.reserve(maxSize_ * maxSize_);
	matrixData_.resize(maxSize_, std::vector<MatrixData>(maxSize_, {}));
	colors.reserve(maxSize_ * maxSize_);
	worldMatrices_.resize(maxSize_, std::vector<Matrix4x4>(maxSize_, Matrix4x4::Identity()));
	for(int i =0; i < maxSize_; i++) {
		for (int j = 0; j < maxSize_; j++) {
			worldMatrices_[i][j] = Matrix::MakeScaleMatrix({ 0.9f, 0.3f, 0.9f }) * Matrix::MakeTranslationMatrix({ static_cast<float>(j), -0.4f, static_cast<float>(i) });
		}
	}
}

void DebugMCRender::Update() {
}

void DebugMCRender::Draw(const Matrix4x4& vpMatrix, std::map<TileType, Vector3> colorMap, const MapChipData& mcData, Window* window) {
	//コピー用データの成型
	colors.clear();
	for (const auto& data : mcData) {
		for (const auto& type : data) {
			colors.push_back(Vector4(colorMap[type], alpha_));
		}
	}

	forGPUData_.clear();
	//world
	for(int i = 0; i < static_cast<int>(mcData.size()); i++) {
		for (int j = 0; j < static_cast<int>(mcData[i].size()); j++) {
			MatrixData md;
			md.world = worldMatrices_[i][j];
			md.vp = vpMatrix;
			forGPUData_.push_back(md);
		}
	}

	if (alpha_ < 0.5f) {
		render_->psoConfig_.depthStencilID = DepthStencilID::Transparent;
	}

	//コピー
	render_->CopyBufferData(0, forGPUData_.data(), sizeof(MatrixData) * forGPUData_.size());
	render_->CopyBufferData(1, colors.data(), sizeof(Vector4) * colors.size());
	render_->instanceNum_ = static_cast<uint32_t>(forGPUData_.size());

	//描画
	render_->Draw(window);
}
