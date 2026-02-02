#include "StaticObjectRender.h"
#include <LightManager.h>

void StaticObjectRender::Initialize(ModelManager* modelManager, DrawDataManager* drawDataManager, bool debugMode) {
	modelManager_ = modelManager;
	drawDataManager_ = drawDataManager;
	debugMode_ = debugMode;

	auto values = binaryManager_.Read(waveFileName_);
	int index = 0;
	if (values.empty()) {
		return;
	}
	int num = BinaryManager::Reverse<int>(values[index++].get());
	waveData_.resize(num);
	for(int i = 0; i < num; i++) {
		for (int j = 0; j < 8; ++j) {
			waveData_[i].waves[j].direction = BinaryManager::Reverse<Vector2>(values[index++].get());
			waveData_[i].waves[j].amplitude = BinaryManager::Reverse<float>(values[index++].get());
			waveData_[i].waves[j].wavelength = BinaryManager::Reverse<float>(values[index++].get());
			waveData_[i].waves[j].speed = BinaryManager::Reverse<float>(values[index++].get());
		}
	}
}

void StaticObjectRender::Draw(const Matrix4x4& vpMatrix, Window* window) {
	if (currentStage_ >= waveData_.size()) {
		waveData_.resize(currentStage_ + 1);
	}

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
		if (modelIndex == wallIndex_) {
			//壁は後で描画する
			continue;
		}

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

	auto it = objects_.find(wallIndex_);
	if (it != objects_.end()) {

		auto render = it->second.get();

		std::vector<Vector4> localPos;
		for (const auto& mat : vsData_[wallIndex_]) {
			Vector4 pos = { mat.world.m[3][0], mat.world.m[3][1], mat.world.m[3][2], 1.0f };
			localPos.push_back(pos);
		}

		waveData_[currentStage_].world = Matrix::MakeScaleMatrix({ 0.99f, 1.0f, 0.99f });
		waveData_[currentStage_].vp = vpMatrix;
		waveData_[currentStage_].cameraPos = { 0.0f, 0.0f, 0.0f };
		waveData_[currentStage_].waveCount = 8;

		render->CopyBufferData(0, &waveData_[currentStage_], sizeof(WaveVS));
		render->CopyBufferData(1, localPos.data(), sizeof(Vector4) * localPos.size());
		render->CopyBufferData(2, &materialData_[wallIndex_], sizeof(Material));
		render->CopyBufferData(3, &lightConfig, sizeof(LightConfig));
		render->CopyBufferData(4, &directionalLights_[1], sizeof(DirectionalLight) * lightConfig.directionalLightNum);
		render->CopyBufferData(5, pointLights_.data(), sizeof(PointLight) * lightConfig.pointLightNum);
		render->Draw(window);
	}
}

void StaticObjectRender::DrawImGui() {
#ifdef USE_IMGUI

	ImGui::Begin("Wave");
	ImGui::BeginChild("Wave Settings", { 100, 0 });
	static int editIndex = 0;
	for (int i = 0; i < 8; i++) {
		std::string label = "Wave " + std::to_string(i);
		if (ImGui::Selectable(label.c_str(), editIndex == i)) {
			editIndex = i;
		}
	}
	ImGui::EndChild();
	ImGui::SameLine();
	ImGui::BeginChild("Wave Edit", { 0, 0 });
	Wave& wave = waveData_[currentStage_].waves[editIndex];
	ImGui::DragFloat2("Direction", &wave.direction.x, 0.01f);
	wave.direction = wave.direction.Normalize();
	ImGui::DragFloat("Amplitude", &wave.amplitude, 0.1f);
	ImGui::DragFloat("Wavelength", &wave.wavelength, 0.1f);
	ImGui::DragFloat("Speed", &wave.speed, 0.1f);
	ImGui::DragFloat("Time", &waveData_[currentStage_].time, 0.1f);
	ImGui::EndChild();
	ImGui::End();

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

	int wallCount = 0;

	for (auto& [modelIndex, transforms] : objects) {
		if (transforms.empty()) {
			continue;
		}

		if (modelIndex == wallIndex_) {
			//壁は後で特別に扱う
			wallCount = int(transforms.size());
			for (int i = 0; i < wallCount; i++) {
				VSData& data = vsData_[modelIndex].emplace_back();
				data.world = Matrix::MakeAffineMatrix(transforms[i].scale, transforms[i].rotate, transforms[i].position);
			}
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

		if (modelIndex == wallIndex_) {
			//壁は後で初期化する
			continue;
		}

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

	//壁オブジェクトの初期化
	if (wallCount >= 1) {

		if (objects_[wallIndex_]) {
			//すでにオブジェクトが存在する場合はスルー
			objects_[wallIndex_]->instanceNum_ = uint32_t(wallCount);
			return;
		}

		objects_[wallIndex_] = std::make_unique<RenderObject>("StaticObjectRender_Wall_" + std::to_string(wallIndex_));
		auto render = objects_[wallIndex_].get();
		auto drawData = drawDataManager_->GetDrawData(modelManager_->GetNodeModelData(wallIndex_).drawDataIndex);

		if (debugMode_) {
			wallCount = 2048;
		}

		render->Initialize();
		render->instanceNum_ = uint32_t(wallCount);
		render->CreateCBV(sizeof(WaveVS), ShaderType::VERTEX_SHADER, "Wave");
		render->CreateSRV(sizeof(Vector4), wallCount, ShaderType::VERTEX_SHADER, "localPos");
		render->CreateCBV(sizeof(Material), ShaderType::PIXEL_SHADER, "MaterialData");
		render->CreateCBV(sizeof(LightConfig), ShaderType::PIXEL_SHADER, "LightConfig");
		render->CreateSRV(sizeof(DirectionalLight), 8, ShaderType::PIXEL_SHADER, "DirectionalLights");
		render->CreateSRV(sizeof(PointLight), 32, ShaderType::PIXEL_SHADER, "PointLights");
		render->SetDrawData(drawData);
		render->SetUseTexture(true);
		render->psoConfig_.vs = "Water/WaterPlane.VS.hlsl";
		render->psoConfig_.ps = "Model/Obj.PS.hlsl";

		NodeModelData modelData = modelManager_->GetNodeModelData(wallIndex_);
		int materialIndex = modelData.materialIndex[0];
		materialData_[wallIndex_].color = modelData.materials[materialIndex].color;
		materialData_[wallIndex_].textureIndex = modelData.materials[materialIndex].textureIndex;
	}
}

void StaticObjectRender::SaveWaveData() {
	int num = static_cast<int>(waveData_.size());
	binaryManager_.RegisterOutput(num);
	for(const auto& waveVS : waveData_) {
		for (int j = 0; j < 8; ++j) {
			binaryManager_.RegisterOutput(waveVS.waves[j].direction);
			binaryManager_.RegisterOutput(waveVS.waves[j].amplitude);
			binaryManager_.RegisterOutput(waveVS.waves[j].wavelength);
			binaryManager_.RegisterOutput(waveVS.waves[j].speed);
		}
	}
	binaryManager_.Write(waveFileName_);
}
