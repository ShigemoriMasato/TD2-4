#include "Gate.h"

Gate::~Gate() {
	Save();
}

void Gate::Initialize(SHEngine::Engine* engine, const LightData& lightData) {
	auto mm = engine->GetModelManager();
	auto ddm = engine->GetDrawDataManager();

	container_ = std::make_unique<SHEngine::BufferContainer>();

	{
		auto modelData = mm->GetNodeModelData(mm->LoadModel("Gate"));
		auto drawData = ddm->GetDrawData(modelData.drawDataIndex);
		gate_ = std::make_unique<SHEngine::Renderer>(drawData);
		gate_->SetVS("Model/Obj.VS.hlsl");
		gate_->SetPS("Model/Obj.PS.hlsl");

		gateBuffer_ = container_->Create(BufferType::SRV, sizeof(VSData), 1);
		auto index = container_->Create(BufferType::SRV, sizeof(uint32_t), (uint32_t)modelData.materialIndex.size());
		auto materials = container_->Create(BufferType::SRV, sizeof(SimpleMaterial), (uint32_t)modelData.materials.size());

		index->CopyBuffer(modelData.materialIndex.data(), sizeof(uint32_t) * modelData.materialIndex.size());
		std::vector<SimpleMaterial> simpleMaterials(modelData.materials.size());
		for (size_t i = 0; i < modelData.materials.size(); i++) {
			simpleMaterials[i].baseColor = modelData.materials[i].color;
			simpleMaterials[i].textureIndex = modelData.materials[i].textureIndex;
		}
		materials->CopyBuffer(simpleMaterials.data(), sizeof(SimpleMaterial) * simpleMaterials.size());
		
		gate_->SetGPUBuffer(gateBuffer_, ShaderType::VERTEX_SHADER, BufferType::SRV);
		gate_->SetGPUBuffer(index, ShaderType::VERTEX_SHADER, BufferType::SRV);
		gate_->SetGPUBuffer(lightData.config, ShaderType::PIXEL_SHADER, BufferType::CBV);
		gate_->SetGPUBuffer(lightData.directional, ShaderType::PIXEL_SHADER, BufferType::SRV);
		gate_->SetGPUBuffer(lightData.point, ShaderType::PIXEL_SHADER, BufferType::SRV);
		gate_->SetGPUBuffer(materials, ShaderType::PIXEL_SHADER, BufferType::SRV);

		gate_->SetUseTexture(true);
		gate_->SetSampler(SHEngine::PSO::SamplerID::Default);


		auto tm = engine->GetTextureManager();
		first_ = tm->GetTextureData(modelData.materials[1].textureIndex);
		second_ = tm->GetTextureData(modelData.materials[2].textureIndex);

		modelData = mm->GetNodeModelData(mm->LoadModel("Wall"));
		drawData = ddm->GetDrawData(modelData.drawDataIndex);

		wall_ = std::make_unique<SHEngine::Renderer>(drawData);
		wall_->SetVS("Model/Obj.VS.hlsl");
		wall_->SetPS("Model/Obj.PS.hlsl");

		auto w_index = container_->Create(BufferType::SRV, sizeof(uint32_t), (uint32_t)modelData.materialIndex.size());
		auto w_materials = container_->Create(BufferType::SRV, sizeof(SimpleMaterial), (uint32_t)modelData.materials.size());

		w_index->CopyBuffer(modelData.materialIndex.data(), sizeof(uint32_t) * modelData.materialIndex.size());
		std::vector<SimpleMaterial> w_simpleMaterials(modelData.materials.size());
		for (size_t i = 0; i < modelData.materials.size(); i++) {
			w_simpleMaterials[i].baseColor = modelData.materials[i].color;
			w_simpleMaterials[i].textureIndex = modelData.materials[i].textureIndex;
		}
		w_materials->CopyBuffer(w_simpleMaterials.data(), sizeof(SimpleMaterial) * w_simpleMaterials.size());

		wall_->SetGPUBuffer(gateBuffer_, ShaderType::VERTEX_SHADER, BufferType::SRV);
		wall_->SetGPUBuffer(w_index, ShaderType::VERTEX_SHADER, BufferType::SRV);
		wall_->SetGPUBuffer(lightData.config, ShaderType::PIXEL_SHADER, BufferType::CBV);
		wall_->SetGPUBuffer(lightData.directional, ShaderType::PIXEL_SHADER, BufferType::SRV);
		wall_->SetGPUBuffer(lightData.point, ShaderType::PIXEL_SHADER, BufferType::SRV);
		wall_->SetGPUBuffer(w_materials, ShaderType::PIXEL_SHADER, BufferType::SRV);

		wall_->SetUseTexture(true);
		wall_->SetSampler(SHEngine::PSO::SamplerID::Default);
	}

	{
		auto modelData = mm->GetNodeModelData(mm->LoadModel("Door"));
		auto drawData = ddm->GetDrawData(modelData.drawDataIndex);
		door_ = std::make_unique<SHEngine::Renderer>(drawData);
		door_->SetVS("Model/Obj.VS.hlsl");
		door_->SetPS("Model/Obj.PS.hlsl");

		doorBuffer_ = container_->Create(BufferType::SRV, sizeof(VSData), 1);
		auto index = container_->Create(BufferType::SRV, sizeof(uint32_t), (uint32_t)modelData.materialIndex.size());
		auto materials = container_->Create(BufferType::SRV, sizeof(SimpleMaterial), (uint32_t)modelData.materials.size());

		index->CopyBuffer(modelData.materialIndex.data(), sizeof(uint32_t) * modelData.materialIndex.size());
		std::vector<SimpleMaterial> simpleMaterials(modelData.materials.size());
		for (size_t i = 0; i < modelData.materials.size(); i++) {
			simpleMaterials[i].baseColor = modelData.materials[i].color;
			simpleMaterials[i].textureIndex = modelData.materials[i].textureIndex;
		}

		materials->CopyBuffer(simpleMaterials.data(), sizeof(SimpleMaterial) * simpleMaterials.size());
		door_->SetGPUBuffer(doorBuffer_, ShaderType::VERTEX_SHADER, BufferType::SRV);
		door_->SetGPUBuffer(index, ShaderType::VERTEX_SHADER, BufferType::SRV);
		door_->SetGPUBuffer(lightData.config, ShaderType::PIXEL_SHADER, BufferType::CBV);
		door_->SetGPUBuffer(lightData.directional, ShaderType::PIXEL_SHADER, BufferType::SRV);
		door_->SetGPUBuffer(lightData.point, ShaderType::PIXEL_SHADER, BufferType::SRV);
		door_->SetGPUBuffer(materials, ShaderType::PIXEL_SHADER, BufferType::SRV);

		door_->SetUseTexture(true);
		door_->SetSampler(SHEngine::PSO::SamplerID::Default);
	}

	Load();

	Reset();
}

void Gate::Update(float deltaTime, Camera* camera) {

	if (open_) {
		timer_ += speed_ * deltaTime;
		speed_ += accel_ * deltaTime;
		speed_ = std::min(speed_, maxSpeed_);
		doorTransform_.position.y += speed_ * deltaTime;
		doorTransform_.position.y = std::min(doorTransform_.position.y, topY_);
	}

	//Matrix作成
	VSData data;

	data.world = gateTransform_.GetMatrix();
	data.wvp = data.world * camera->GetVPMatrix();
	gateBuffer_->CopyBuffer(&data, sizeof(VSData));

	data.world = doorTransform_.GetMatrix();
	data.wvp = data.world * camera->GetVPMatrix();
	doorBuffer_->CopyBuffer(&data, sizeof(VSData));

#ifdef USE_IMGUI

	ImGui::Begin("Gate");

	ImGui::Text("Speed: %f, posY: %f", speed_, doorTransform_.position.y);

	ImGui::DragFloat("InitSpeed", &initSpeed_, 0.1f);
	ImGui::DragFloat("MaxSpeed", &maxSpeed_, 0.1f);
	ImGui::DragFloat("Accel", &accel_, 0.1f);

	if(ImGui::Button("Reset")) {
		Reset();
	}
	if (ImGui::Button("Open")) {
		Open();
	}

	ImGui::End();

#endif
}

void Gate::Draw(CmdObj* cmdObj) {
	gate_->Draw(cmdObj);
	wall_->Draw(cmdObj);
	door_->Draw(cmdObj);
}

void Gate::Open() {
	Reset();
	open_ = true;
}

void Gate::Reset() {
	speed_ = initSpeed_;
	timer_ = 0.0f;
	doorTransform_.position.y = 0.0f;
	open_ = false;
}

void Gate::Save() {
	BinaryManager bin;
	const std::string fileName = "GateData.bin";

	bin.Register(&gateTransform_);
	bin.Register(&doorTransform_);
	bin.Register(&initSpeed_);
	bin.Register(&maxSpeed_);
	bin.Register(&accel_);

	bin.Write(fileName);
}

void Gate::Load() {
	BinaryManager bin;
	const std::string fileName = "GateData.bin";
	if (!bin.Boot(fileName)) {
		return;
	}

	gateTransform_ = bin.Reverse<Transform>();
	doorTransform_ = bin.Reverse<Transform>();
	initSpeed_ = bin.Reverse<float>();
	maxSpeed_ = bin.Reverse<float>();
	accel_ = bin.Reverse<float>();
}
