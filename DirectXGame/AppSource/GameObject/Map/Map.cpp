#include "Map.h"
#include <algorithm>
#include <Utility/MatrixFactory.h>
#include <Utility/Color.h>

#ifdef USE_IMGUI
#include <imgui/imgui.h>
#endif

Map::Map() {}

void Map::Initialize(SHEngine::DrawDataManager* drawDataManager, SHEngine::ModelManager* modelManager, const MapInfo& mapInfo) {
	mapInfo_ = mapInfo;

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

		// ステージモデルの読み込み
		stageModelID_ = modelManager_->LoadModel("Assets/Model/Stage");

		// ステージレンダーオブジェクトの初期化
		stageRender_ = std::make_unique<SHEngine::RenderObject>("Map_Stage");
		stageRender_->Initialize();

		auto stageModel = modelManager_->GetNodeModelData(stageModelID_);
		auto stageDrawData = drawDataManager_->GetDrawData(stageModel.drawDataIndex);
		stageRender_->SetDrawData(stageDrawData);

		// ステージのテクスチャインデックスを保存
		if (!stageModel.materials.empty() && !stageModel.materialIndex.empty()) {
			auto& material = stageModel.materials[stageModel.materialIndex.front()];
			stageTextureIndex_ = material.textureIndex;
		}

		stageRender_->psoConfig_.vs = "Simple.VS.hlsl";
		stageRender_->psoConfig_.ps = "TexColor.PS.hlsl";
		stageRender_->psoConfig_.isSwapChain = false;

		stageRender_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "WVP");
		stageRender_->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "Color");
		stageRender_->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");
		stageRender_->SetUseTexture(true);
		stageRender_->instanceNum_ = 1;
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

	// ステージの更新
	if (stageRender_) {
		Matrix4x4 world = Matrix::MakeAffineMatrix(stageScale_, stageRotation_, stagePosition_);
		Matrix4x4 wvp = world * vpMatrix;
		Vector4 color = {1.0f, 1.0f, 1.0f, 1.0f};

		stageRender_->CopyBufferData(0, &wvp, sizeof(wvp));
		stageRender_->CopyBufferData(1, &color, sizeof(color));
		stageRender_->CopyBufferData(2, &stageTextureIndex_, sizeof(stageTextureIndex_));
	}
}

void Map::Draw(CmdObj* cmdObj) {
	if (render_) {
		render_->Draw(cmdObj);
	}

	if (stageRender_) {
		//stageRender_->Draw(cmdObj);
	}
}

bool Map::IsInBounds(const Vector3& position) const {
	return position.x >= mapInfo_.minX && position.x <= mapInfo_.maxX &&
		position.z >= mapInfo_.minZ && position.z <= mapInfo_.maxZ;
}

Vector3 Map::ClampToBounds(const Vector3& position) const {
	Vector3 clamped = position;
	clamped.x = std::clamp(clamped.x, mapInfo_.minX, mapInfo_.maxX);
	clamped.z = std::clamp(clamped.z, mapInfo_.minZ, mapInfo_.maxZ);
	return clamped;
}

void Map::DrawDebugGUI() {
#ifdef USE_IMGUI
	ImGui::Begin("Map Debug");

	if (ImGui::TreeNode("Stage Transform")) {
		ImGui::DragFloat3("Position", &stagePosition_.x, 0.1f);
		ImGui::DragFloat3("Rotation", &stageRotation_.x, 0.01f);
		ImGui::DragFloat3("Scale", &stageScale_.x, 0.01f);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Block Settings")) {
		ImGui::DragFloat3("Block Scale", &blockScale_.x, 0.01f);
		ImGui::TreePop();
	}

	ImGui::End();
#endif
}
