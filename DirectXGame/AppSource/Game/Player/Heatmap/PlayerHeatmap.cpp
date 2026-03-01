#include "PlayerHeatmap.h"
#include <numbers>

using namespace SHEngine;

void Player::HeatmapManager::Initialize(ModelManager* modelManager, DrawDataManager* drawDataManager, GridConfig config) {
	config_ = config;
	data_.assign(config_.width * config_.height, 0.0f);
	worldMatrices_.reserve(maxInstanceNum_);
	colors_.reserve(maxInstanceNum_);

	// RenderObjectの初期化
	render_ = std::make_unique<RenderObject>();
	render_->Initialize();

	// シェーダーを指定
	render_->psoConfig_.vs = "Game/Heatmap.VS.hlsl";
	render_->psoConfig_.ps = "Game/Heatmap.PS.hlsl";

	// CBVの作成
	render_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "ViewProj");

	// SRVの作成
	render_->CreateSRV(sizeof(Matrix4x4), maxInstanceNum_, ShaderType::VERTEX_SHADER, "WorldMatrices");

	// SRVの作成
	render_->CreateSRV(sizeof(Vector4), maxInstanceNum_, ShaderType::PIXEL_SHADER, "Colors");

	// モデルデータの読み込み
	int modelHandle = modelManager->LoadModel("Assets/.EngineResource/Model/Plane");
	auto modelData = modelManager->GetNodeModelData(modelHandle);
	auto drawData = drawDataManager->GetDrawData(modelData.drawDataIndex);
	render_->SetDrawData(drawData);
}

void Player::HeatmapManager::Record(Vector3 position, float deltaTime) {
	// 座標をグリッドインデックスに変換
	int x = static_cast<int>((position.x - config_.origin.x) / config_.cellSize);
	int z = static_cast<int>((position.z - config_.origin.z) / config_.cellSize);

	if (x >= 0 && x < config_.width && z >= 0 && z < config_.height) {
		data_[z * config_.width + x] += deltaTime; // 滞在時間を加算
	}
}

void Player::HeatmapManager::Update(const Matrix4x4& viewProj) {
	worldMatrices_.clear();
	colors_.clear();

	for (int z = 0; z < config_.height; ++z) {
		for (int x = 0; x < config_.width; ++x) {
			float stayTime = data_[z * config_.width + x];

			// 滞在時間が少しでもあるもののみ追加
			if (stayTime >= 0.0f) {
				Vector3 pos;
				pos.x = config_.origin.x + (x * config_.cellSize) + (config_.cellSize * 0.5f);
				pos.y = config_.origin.y + 0.1f;
				pos.z = config_.origin.z + (z * config_.cellSize) + (config_.cellSize * 0.5f);

				Vector3 scale = {config_.cellSize, config_.cellSize, 0.1f};
				Vector3 rotate = {std::numbers::pi_v<float> / 2.0f, 0.0f, 0.0f};

				Matrix4x4 world = Matrix::MakeAffineMatrix(scale, rotate, pos);
				worldMatrices_.push_back(world);

				// 色の計算
				float ratio = std::clamp(stayTime / maxStayTime_, 0.0f, 1.0f);
				Vector4 color = {ratio, 0.0f, 1.0f - ratio, 0.5f};
				colors_.push_back(color);
			}
		}
	}

	// バッファへのコピーと描画数の更新
	if (!worldMatrices_.empty()) {
		render_->CopyBufferData(0, &viewProj, sizeof(Matrix4x4));
		render_->CopyBufferData(1, worldMatrices_.data(), sizeof(Matrix4x4) * worldMatrices_.size());
		render_->CopyBufferData(2, colors_.data(), sizeof(Vector4) * colors_.size());
		render_->instanceNum_ = static_cast<int>(worldMatrices_.size());
	} else {
		render_->instanceNum_ = 0;
	}
}

void Player::HeatmapManager::Draw(CmdObj* cmdObj) {
	if (render_->instanceNum_ > 0) {
		render_->Draw(cmdObj);
	}

#ifdef USE_IMGUI
	ImGui::Begin("Heatmap Debug");
	ImGui::DragFloat("Max Stay Time", &maxStayTime_, 0.1f, 1.0f, 60.0f);
	if (ImGui::Button("Clear Heatmap")) {
		std::fill(data_.begin(), data_.end(), 0.0f);
	}
	ImGui::End();
#endif
}
