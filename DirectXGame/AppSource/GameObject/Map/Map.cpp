#include "Map.h"
#include <algorithm>
#include <Utility/MatrixFactory.h>
#include <Utility/Color.h>

Map::Map() {}

void Map::Initialize(SHEngine::DrawDataManager* drawDataManager, SHEngine::ModelManager* modelManager, float minX, float maxX, float minZ, float maxZ) {
	minX_ = minX;
	maxX_ = maxX;
	minZ_ = minZ;
	maxZ_ = maxZ;

	drawDataManager_ = drawDataManager;
	modelManager_ = modelManager;

	// モデル管理が設定されている場合、グラスブロックを初期化
	if (modelManager_ && drawDataManager_) {
		// グラスブロックモデルの読み込み
		grassModelID_ = modelManager_->LoadModel("Assets/Model/Block/Grass");

		// レンダーオブジェクトの初期化
		render_ = std::make_unique<SHEngine::RenderObject>("Map_GrassBlocks");
		render_->Initialize();
		
		auto model = modelManager_->GetNodeModelData(grassModelID_);
		auto drawData = drawDataManager_->GetDrawData(model.drawDataIndex);
		render_->SetDrawData(drawData);
		
		// テクスチャインデックスを保存
		if (!model.materials.empty() && !model.materialIndex.empty()) {
			auto& material = model.materials[model.materialIndex.front()];
			textureIndex_ = material.textureIndex;
		}
		
		render_->psoConfig_.vs = "Simples.VS.hlsl";
		render_->psoConfig_.ps = "TexColors.PS.hlsl";
		render_->psoConfig_.isSwapChain = false;
		
		// インスタンス数（全ブロック数）
		const int instanceCount = (kMapWidth * kMapDepth <= kMaxInstances) ? kMapWidth * kMapDepth : kMaxInstances;
		render_->CreateSRV(sizeof(Matrix4x4), instanceCount, ShaderType::VERTEX_SHADER, "WVP");
		render_->CreateSRV(sizeof(Vector4), instanceCount, ShaderType::PIXEL_SHADER, "Color");
		render_->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");
		render_->SetUseTexture(true);
		render_->instanceNum_ = instanceCount;
	}
}

void Map::Update(const Matrix4x4& vpMatrix) {
	if (!render_) {
		return;
	}

	// 各ブロックの座標とカラーを計算
	const int instanceCount = kMapWidth * kMapDepth;
	std::vector<Matrix4x4> wvpMatrices;
	std::vector<Vector4> colors;
	
	wvpMatrices.reserve(instanceCount);
	colors.reserve(instanceCount);
	
	// 原点から10x10のブロック配置（モデルサイズ2x2x2なので間隔は2.0f）
	for (int z = 0; z < kMapDepth; ++z) {
		for (int x = 0; x < kMapWidth; ++x) {
			// ブロックのワールド座標（原点から2.0f間隔）
			Vector3 position = { 
				static_cast<float>(x) * kBlockSize, 
				0.0f, 
				static_cast<float>(z) * kBlockSize 
			};
			
			// WVP行列を作成
			Matrix4x4 world = Matrix::MakeAffineMatrix(blockScale_, Vector3(), position);
			Matrix4x4 wvp = world * vpMatrix;
			
			wvpMatrices.push_back(wvp);
			colors.push_back({ 1.0f, 1.0f, 1.0f, 1.0f });
		}
	}

	// バッファにコピー
	render_->CopyBufferData(0, wvpMatrices.data(), sizeof(Matrix4x4) * wvpMatrices.size());
	render_->CopyBufferData(1, colors.data(), sizeof(Vector4) * colors.size());
	render_->CopyBufferData(2, &textureIndex_, sizeof(int));
}

void Map::Draw(CmdObj* cmdObj) {
	if (render_) {
		render_->Draw(cmdObj);
	}
}

bool Map::IsInBounds(const Vector3& position) const {
	return position.x >= minX_ && position.x <= maxX_ &&
		position.z >= minZ_ && position.z <= maxZ_;
}

Vector3 Map::ClampToBounds(const Vector3& position) const {
	Vector3 clamped = position;
	clamped.x = std::clamp(clamped.x, minX_, maxX_);
	clamped.z = std::clamp(clamped.z, minZ_, maxZ_);
	return clamped;
}
