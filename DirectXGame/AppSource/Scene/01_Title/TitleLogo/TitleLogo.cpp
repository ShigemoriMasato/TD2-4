#include "TitleLogo.h"
#include <Tool/Binary/BinaryManager.h>

TitleLogo::~TitleLogo() {
	Save();
}

void TitleLogo::Initialize(SHEngine::Engine* enigne, CmdObj* compute) {
	auto tm = enigne->GetTextureManager();
	int textureIndex = tm->LoadTexture("Title/Logo.png");
	auto textureData = tm->GetTextureData(textureIndex);

	constexpr int kParticleNum = 1000000;
	container_ = std::make_unique<SHEngine::BufferContainer>();
	auto pool = container_->Create(BufferType::UAV, sizeof(uint32_t), kParticleNum, 1);
	auto poolIndex = container_->Create(BufferType::UAV, sizeof(uint32_t), 1, 1);
	auto isUse = container_->Create(BufferType::UAV, sizeof(int), kParticleNum, 1);
	auto c_positions = container_->Create(BufferType::UAV, sizeof(Vector3), kParticleNum, 1);
	auto c_velocity = container_->Create(BufferType::UAV, sizeof(Vector3), kParticleNum, 1);
	auto c_lifeTimes = container_->Create(BufferType::UAV, sizeof(float), kParticleNum, 1);

	auto d_positions = container_->Create(BufferType::SRV_UAV, sizeof(Vector3), kParticleNum);
	auto d_colors = container_->Create(BufferType::SRV_UAV, sizeof(Vector4), kParticleNum);
	
	auto particleNum = container_->Create(BufferType::CBV, sizeof(int), 1, 1);
	particleNum->CopyBuffer(&kParticleNum, sizeof(kParticleNum));

	emitBuffer_ = container_->Create(BufferType::CBV, sizeof(EmitData));
	updateBuffer_ = container_->Create(BufferType::CBV, sizeof(UpdateData));
	sizeBuffer_ = container_->Create(BufferType::CBV, sizeof(float));
	matrixBuffer_ = container_->Create(BufferType::CBV, sizeof(MatrixData));
	waveBuffer_ = container_->Create(BufferType::CBV, sizeof(Wave) * 16);

	init_ = std::make_unique<SHEngine::ComputeObject>();
	init_->SetShader("Particle/TitleLogo/Initialize.CS.hlsl");
	init_->SetGPUBuffers(BufferType::UAV, { pool, poolIndex, isUse });
	init_->SetGPUBuffer(BufferType::CBV, particleNum);
	init_->SetThreadGroupSize(kParticleNum / 256 + 1, 1, 1);
	init_->Execute(compute);

	emit_ = std::make_unique<SHEngine::ComputeObject>();
	emit_->SetShader("Particle/TitleLogo/Emit.CS.hlsl");
	emit_->SetGPUBuffers(BufferType::UAV, { pool, poolIndex, isUse, c_positions, c_velocity, c_lifeTimes });
	emit_->SetGPUBuffer(BufferType::CBV, emitBuffer_);
	emit_->SetUseTexture(true);
	emit_->SetSamplerID(SHEngine::PSO::SamplerID::Default);

	update_ = std::make_unique<SHEngine::ComputeObject>();
	update_->SetShader("Particle/TitleLogo/Update.CS.hlsl");
	update_->SetGPUBuffers(BufferType::UAV, { pool, poolIndex, d_positions, d_colors, c_velocity, c_lifeTimes, c_positions, isUse });
	update_->SetGPUBuffers(BufferType::CBV, { particleNum, updateBuffer_, waveBuffer_ });
	update_->SetThreadGroupSize(kParticleNum / 256 + 1, 1, 1);


	auto ddm = enigne->GetDrawDataManager();
	auto mm = enigne->GetModelManager();
	auto drawData = ddm->GetDrawData(mm->GetNodeModelData(1).drawDataIndex);

	renderer_ = std::make_unique<SHEngine::Renderer>(drawData);
	renderer_->SetVS("Particle/Title.VS.hlsl");
	renderer_->SetPS("Particle/Title.PS.hlsl");
	renderer_->SetGPUBuffer(sizeBuffer_, ShaderType::VERTEX_SHADER, BufferType::CBV);
	renderer_->SetGPUBuffer(matrixBuffer_, ShaderType::VERTEX_SHADER, BufferType::CBV);
	renderer_->SetGPUBuffer(d_positions, ShaderType::VERTEX_SHADER, BufferType::SRV);
	renderer_->SetGPUBuffer(d_colors, ShaderType::PIXEL_SHADER, BufferType::SRV);
	renderer_->instanceNum_ = kParticleNum;

	waves_.resize(16);
	emitData_.textureIndex = textureIndex;

	Load();
}

void TitleLogo::Update(float deltaTime, Camera* camera, CmdObj* compute) {
	emitData_.seed = randomDist_(randomEngine_);
	emitBuffer_->CopyBuffer(&emitData_, sizeof(emitData_));

	updateData_.deltaTime = deltaTime;
	updateData_.fieldSize = emitData_.fieldSize;
	updateData_.lifetime = emitData_.lifeTime;
	updateData_.worldMatrix = parentTransform_.GetMatrix();
	updateBuffer_->CopyBuffer(&updateData_, sizeof(updateData_));

	matrixData_.vpMatrix = camera->GetVPMatrix();
	matrixData_.billboardMatrix = camera->GetBillboardMatrix();
	matrixBuffer_->CopyBuffer(&matrixData_, sizeof(matrixData_));

	sizeBuffer_->CopyBuffer(&size_, sizeof(float));

	for (auto& wave : waves_) {
		wave.lifetime += deltaTime;
	}
	waveBuffer_->CopyBuffer(waves_.data(), sizeof(Wave) * waves_.size());

	emit_->SetThreadGroupSize(emitData_.emitNum / 64 + 1, 1, 1);
	
	emit_->Execute(compute);
	update_->Execute(compute);


#ifdef USE_IMGUI

	ImGui::Begin("TitleLogo");

	ImGui::Checkbox("Save", &isSave_);
	ImGui::Separator();

	ImGui::DragFloat3("FieldSize", &emitData_.fieldSize.x, 1.0f, 0.0f);
	ImGui::DragFloat("Speed", &emitData_.speed, 0.1f, 0.0f);
	ImGui::DragFloat("LifeTime", &emitData_.lifeTime, 0.1f, 0.0f);
	ImGui::DragInt("EmitNum", &emitData_.emitNum, 1.0f, 0);
	ImGui::ColorEdit3("Color", &updateData_.color.x);
	ImGui::DragFloat("Size", &size_, 0.001f, 0.001f, 1.0f);

	ImGui::DragFloat3("Scale", &parentTransform_.scale.x, 0.01f);
	ImGui::DragFloat3("Rotate", &parentTransform_.rotate.x, 0.01f);
	ImGui::DragFloat3("Position", &parentTransform_.position.x, 0.1f);

	ImGui::End();

#endif
}

void TitleLogo::Draw(CmdObj* direct) {
	renderer_->Draw(direct);
}

void TitleLogo::AddWave(const Wave& wave) {
	for (size_t i = 0; i < waves_.size(); ++i) {
		if (waves_[i].lifetime > waves_[i].maxlifetime) {
			waves_[i] = wave;
			return;
		}
	}
}


void TitleLogo::Save() {
	if (!isSave_) {
		return;
	}

	BinaryManager bin;
	const std::string saveFile = "TitleLogoData.bin";

	bin.Register<Vector3>(&emitData_.fieldSize);
	bin.Register<float>(&emitData_.speed);
	bin.Register<float>(&emitData_.lifeTime);
	bin.Register<int>(&emitData_.emitNum);
	bin.Register<Vector3>(&updateData_.color);
	bin.Register<float>(&size_);
	bin.Register<Transform>(&parentTransform_);
	bin.Write(saveFile);
}

void TitleLogo::Load() {
	BinaryManager bin;
	const std::string saveFile = "TitleLogoData.bin";

	if (!bin.Boot(saveFile)) {
		return;
	}

	emitData_.fieldSize = bin.Reverse<Vector3>();
	emitData_.speed = bin.Reverse<float>();
	emitData_.lifeTime = bin.Reverse<float>();
	emitData_.emitNum = bin.Reverse<int>();
	updateData_.color = bin.Reverse<Vector3>();
	size_ = bin.Reverse<float>();
	parentTransform_ = bin.Reverse<Transform>();
}
