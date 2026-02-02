#include "StaticObjectRender.h"
#include <LightManager.h>

void StaticObjectRender::Initialize(ModelManager* modelManager, DrawDataManager* drawDataManager, bool debugMode) {
	modelManager_ = modelManager;
	drawDataManager_ = drawDataManager;
	debugMode_ = debugMode;
}

void StaticObjectRender::Draw(const Matrix4x4& vpMatrix, Window* window) {
	//行列の更新
	for (auto& [modelIndex, vsData] : vsData_) {
		for (auto& mat : vsData) {
			mat.wvp = mat.world * vpMatrix;
		}
	}

	auto inst = LightManager::GetInstance();
	auto directionalLights_ = inst->GetDirLights();
	auto pointLights_ = inst->GetPointLights();

	LightConfig lightConfig{};
	lightConfig.directionalLightNum = static_cast<int>(directionalLights_.size() - 1);
	lightConfig.pointLightNum = static_cast<int>(pointLights_.size());

	for (auto& [modelIndex, render] : objects_) {
		Material mat = materialData_[modelIndex];
		NodeModelData modelData = modelManager_->GetNodeModelData(modelIndex);

		int materialIndex = modelData.materialIndex[0];
		mat.color = modelData.materials[materialIndex].color;
		mat.color.w = 1.0f;
		mat.textureIndex = modelData.materials[materialIndex].textureIndex;
		render->CopyBufferData(1, &mat, sizeof(Material));

		render->CopyBufferData(0, vsData_[modelIndex].data(), sizeof(VSData) * vsData_[modelIndex].size());
		//ライトの情報を渡す
		render->CopyBufferData(2, &lightConfig, sizeof(LightConfig));
		render->CopyBufferData(3, &directionalLights_[1], sizeof(DirectionalLight) * lightConfig.directionalLightNum);
		render->CopyBufferData(4, pointLights_.data(), sizeof(PointLight) * lightConfig.pointLightNum);

		render->Draw(window);
	}
}

void StaticObjectRender::DrawImGui() {
#ifdef USE_IMGUI

#endif
}

void StaticObjectRender::SetAlpha(float alpha) {
	for (auto& [modelIndex, render] : objects_) {
		Material mat = materialData_[modelIndex];
		mat.color.w = alpha;
		render->CopyBufferData(1, &mat, sizeof(Material));
	}
}

void StaticObjectRender::SetObjects(const std::map<int, std::vector<Transform>>& objects) {
	// オブジェクトの初期化
	for (auto& [modelIndex, data] : vsData_) {
		data.clear();
	}

	for (auto& [modelIndex, transforms] : objects) {
		if (transforms.empty()) {
			continue;
		}

		const auto& it = objects_.find(modelIndex);

		//すでにオブジェクトが存在する場合はworldMatrixだけ追加する
		if (it != objects_.end()) {
			for (const auto& transform : transforms) {
				VSData& data = vsData_[modelIndex].emplace_back();
				data.world = Matrix::MakeAffineMatrix(transform.scale, transform.rotate, transform.position);
			}
			continue;
		}

		//無い場合はRenderObjectを生成
		auto render = std::make_unique<RenderObject>("StaticObjectRender_" + std::to_string(modelIndex));
		render->instanceNum_ = 0xffffffff;
		NodeModelData modelData = modelManager_->GetNodeModelData(modelIndex);
		DrawData drawData = drawDataManager_->GetDrawData(modelData.drawDataIndex);
		//初期化は数を集計した後にやる
		objects_[modelIndex] = std::move(render);

		for (auto& transform : transforms) {
			VSData& data = vsData_[modelIndex].emplace_back();
			data.world = Matrix::MakeAffineMatrix(transform.scale, transform.rotate, transform.position);
		}

		int materialIndex = modelData.materialIndex[0];
		materialData_[modelIndex].color = modelData.materials[materialIndex].color;
		materialData_[modelIndex].textureIndex = modelData.materials[materialIndex].textureIndex;
	}

	for (auto& [modelIndex, render] : objects_) {

		if (render->instanceNum_ != 0xffffffff) {
			//描画数だけ合わせる
			render->instanceNum_ = static_cast<int>(vsData_[modelIndex].size());
			continue;
		}

		int generateNum = static_cast<int>(vsData_[modelIndex].size());
		if (debugMode_) {
			generateNum = 128;
		}

		NodeModelData modelData = modelManager_->GetNodeModelData(modelIndex);
		DrawData drawData = drawDataManager_->GetDrawData(modelData.drawDataIndex);

		render->Initialize();
		render->instanceNum_ = static_cast<int>(vsData_[modelIndex].size());
		render->SetDrawData(drawData);
		render->CreateSRV(sizeof(VSData), generateNum, ShaderType::VERTEX_SHADER, "Matrices");
		render->CreateCBV(sizeof(Material), ShaderType::PIXEL_SHADER, "MaterialData");
		render->CreateCBV(sizeof(LightConfig), ShaderType::PIXEL_SHADER, "LightConfig");
		render->CreateSRV(sizeof(DirectionalLight), 8, ShaderType::PIXEL_SHADER, "DirectionalLights");
		render->CreateSRV(sizeof(PointLight), 32, ShaderType::PIXEL_SHADER, "PointLights");
		render->psoConfig_.vs = "Model/Obj.VS.hlsl";
		render->psoConfig_.ps = "Model/Obj.PS.hlsl";
		render->SetUseTexture(true);

	}
}
