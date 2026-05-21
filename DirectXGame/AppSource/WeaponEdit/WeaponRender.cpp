#include "WeaponRender.h"

WeaponRender::WeaponRender(SHEngine::Engine* engine) {
	auto ddm = engine->GetDrawDataManager();
	auto dummyDrawData = ddm->GetDrawData(0);

	container_ = std::make_unique<SHEngine::BufferContainer>(dummyDrawData);
	dirLightBuffer_ = container_->Create(BufferType::SRV, sizeof(DirectionalLight), maxNum_);
	pointLightBuffer_ = container_->Create(BufferType::SRV, sizeof(PointLight), maxNum_);
	lightNumBuffer_ = container_->Create(BufferType::CBV, sizeof(LightNumData));

	dirLights_.resize(maxNum_);
	pointLights_.resize(maxNum_);
}

int WeaponRender::AddRenderData(const Weapon::RenderData& renderData) {
	return CreateRenderer(renderData);
}

void WeaponRender::DeleteRenderer(int id) {
	renderers_.erase(id);
	gpuBuffers_.erase(id);
}

//==============================================================================================================


void WeaponRender::DrawImGui() {
#ifdef USE_IMGUI

	ImGui::Begin("Light Config");

	if (ImGui::TreeNode("Directional Light")) {
		static int currentDirNum = 0;
		ImGui::Text("Editing Light: %d", currentDirNum);
		ImGui::SameLine();
		if (ImGui::Button("-")) {
			currentDirNum = std::max(0, currentDirNum - 1);
		}
		ImGui::SameLine();
		if (ImGui::Button("+")) {
			currentDirNum = std::min(maxNum_, currentDirNum + 1);
		}

		ImGui::Separator();

		ImGui::DragFloat3("Direction", &dirLights_[currentDirNum].direction.x, 0.1f);
		ImGui::ColorEdit3("Color", &dirLights_[currentDirNum].color.x);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Point Light")) {
		static int currentPointNum = 0;
		ImGui::Text("Editing Light: %d", currentPointNum);
		ImGui::SameLine();
		if (ImGui::Button("-##point")) {
			currentPointNum = std::max(0, currentPointNum - 1);
		}
		ImGui::SameLine();
		if (ImGui::Button("+##point")) {
			currentPointNum = std::min(maxNum_, currentPointNum + 1);
		}
		ImGui::Separator();
		ImGui::DragFloat3("Position", &pointLights_[currentPointNum].position.x, 0.1f);
		ImGui::ColorEdit3("Color##point", &pointLights_[currentPointNum].color.x);
		ImGui::DragFloat("Radius", &pointLights_[currentPointNum].radius, 0.1f, 0.0f, FLT_MAX);
		ImGui::DragFloat("Intensity", &pointLights_[currentPointNum].intensity, 0.1f, 0.0f, FLT_MAX);
		ImGui::TreePop();

	}

	ImGui::End();

#endif // USE_IMGUI
}


//=============================================================================================================


int WeaponRender::CreateRenderer(Weapon::RenderData renderData) {
	int id = nextID_++;
	auto& r = renderers_[id];
	auto& wvpBuf = gpuBuffers_[id];

	r = std::make_unique<SHEngine::Renderer>(renderData.drawData);
	r->SetVS("Model/Obj.VS.hlsl");
	r->SetPS("Model/Obj.PS.hlsl");

	wvpBuf = container_->Create(BufferType::CBV, sizeof(VSData));
	auto materialBuf = container_->Create(BufferType::CBV, sizeof(MaterialData));
	
	r->SetGPUBuffer(wvpBuf, ShaderType::VERTEX_SHADER, BufferType::CBV);
	r->SetGPUBuffer(materialBuf, ShaderType::PIXEL_SHADER, BufferType::CBV);
	r->SetGPUBuffer(lightNumBuffer_, ShaderType::PIXEL_SHADER, BufferType::CBV);
	r->SetGPUBuffer(dirLightBuffer_, ShaderType::PIXEL_SHADER, BufferType::SRV);
	r->SetGPUBuffer(pointLightBuffer_, ShaderType::PIXEL_SHADER, BufferType::SRV);

	r->SetUseTexture(true);

	//Materialは定数なので、最初に作成しておく
	{
		MaterialData data = {};
		Material material = renderData.modelData.materials[renderData.modelData.materialIndex.front()];
		data.color = material.color;
		data.textureIndex = material.textureIndex;
		materialBuf->CopyBuffer(&data, sizeof(data));
	}

	return id;
}
