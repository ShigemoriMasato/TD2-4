#include "DebugMCRender.h"
#include <Utility/Easing.h>
#include <algorithm>

void DebugMCRender::Initialize(MapChipData* data, const DrawData& drawData) {
	render_ = std::make_unique<RenderObject>();
	render_->Initialize();
	render_->CreateSRV(sizeof(MatrixData), maxSize_ * maxSize_, ShaderType::VERTEX_SHADER, "WVP");
	render_->CreateSRV(sizeof(Vector4), maxSize_ * maxSize_, ShaderType::PIXEL_SHADER, "Color");
	render_->SetDrawData(drawData);
	render_->psoConfig_.vs = "Simples.VS.hlsl";
	render_->psoConfig_.ps = "DebugBlocks.PS.hlsl";

	mcData_ = data;

	for (int i = 0; i < static_cast<int>(TileType::Count); ++i) {
		Vector4 color = ConvertColor(lerpColor(0xffbb88ff, 0x88bbffff, (float)i / (float)(static_cast<int>(TileType::Count) - 1)));
		colorMap_[static_cast<TileType>(i)] = { color.x, color.y, color.z };
	}

	//worldMatrixの初期化
	forGPUData_.reserve(maxSize_ * maxSize_);
	matrixData_.resize(maxSize_, std::vector<MatrixData>(maxSize_, {}));
	colors.reserve(maxSize_ * maxSize_);
}

void DebugMCRender::Update() {
	matrixData_.resize(mcData_->size());
	for (size_t z = 0; z < mcData_->size(); ++z) {
		matrixData_[z].resize(mcData_->at(z).size());
		for (size_t x = 0; x < mcData_->at(z).size(); ++x) {
			matrixData_[z][x].world = Matrix::MakeAffineMatrix({ 1.0f, 0.5f, 1.0f }, {}, { float(x) + 0.5f, 0.0f, float(z) + 0.5f });
		}
	}
}

void DebugMCRender::Draw(const Matrix4x4& vpMatrix, Window* window) {
	//コピー用データの成型
	colors.clear();
	for (const auto& data : *mcData_) {
		for (const auto& type : data) {
			colors.push_back(Vector4(colorMap_.at(type), alpha_));
		}
	}

	forGPUData_.clear();
	//world
	for (const auto& row : matrixData_) {
		forGPUData_.insert(forGPUData_.end(), row.begin(), row.end());
	}
	//vp
	for (auto& p : forGPUData_) {
		p.vp = vpMatrix;
	}

	//コピー
	render_->CopyBufferData(0, forGPUData_.data(), sizeof(MatrixData) * forGPUData_.size());
	render_->CopyBufferData(1, colors.data(), sizeof(Vector4) * colors.size());
	render_->instanceNum_ = static_cast<uint32_t>(forGPUData_.size());

	//描画
	render_->Draw(window);
}

void DebugMCRender::EditColor(TileType type, const Vector3& color) {
	colorMap_[type] = color;
}
