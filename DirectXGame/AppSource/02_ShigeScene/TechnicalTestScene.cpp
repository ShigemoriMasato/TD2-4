#include "TechnicalTestScene.h"

using namespace SHEngine;

void TechnicalTestScene::Initialize() {
	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize(input_);

	constexpr uint32_t kParticleCount = 64;
	
	auto wvp = bufferCont_.Create(BufferType::SRV_UAV, sizeof(Matrix4x4), kParticleCount);
	auto camera = bufferCont_.Create(BufferType::CBV, sizeof(Matrix4x4));

	auto initConfig = bufferCont_.Create(BufferType::CBV, sizeof(InitConfig));
	auto size = bufferCont_.Create(BufferType::CBV, sizeof(float));
	auto emitConfig = bufferCont_.Create(BufferType::CBV, sizeof(EmitConfig));
	auto deadTime = bufferCont_.Create(BufferType::CBV, sizeof(float));
	auto deltaTime = bufferCont_.Create(BufferType::CBV, sizeof(float));
	auto particleNum = bufferCont_.Create(BufferType::CBV, sizeof(uint32_t));
	particleNum->CopyBuffer(&kParticleCount, sizeof(kParticleCount));

	auto positions = bufferCont_.Create(BufferType::UAV, sizeof(Vector3), kParticleCount);
	auto velocities = bufferCont_.Create(BufferType::UAV, sizeof(Vector3), kParticleCount);
	auto lifeTimes = bufferCont_.Create(BufferType::UAV, sizeof(float), kParticleCount);

	auto freeList = bufferCont_.Create(BufferType::UAV, sizeof(uint32_t), kParticleCount);
	auto freeListIndex = bufferCont_.Create(BufferType::UAV, sizeof(uint32_t));

	particleInit_ = std::make_unique<SHEngine::ComputeObject>("Initialize");
	particleInit_->SetShader("Particle/Test/Initialize.CS.hlsl");
	particleInit_->SetThreadGroupSize(256, 1, 1);
	particleInit_->SetGPUBuffer(freeList, BufferType::UAV);
	particleInit_->SetGPUBuffer(freeListIndex, BufferType::UAV);
	particleInit_->SetGPUBuffer(wvp, BufferType::UAV);
	particleInit_->SetGPUBuffer(lifeTimes, BufferType::UAV);
	particleInit_->SetGPUBuffer(particleNum, BufferType::CBV);
	particleInit_->SetGPUBuffer(deadTime, BufferType::CBV);

	particleEmit_ = std::make_unique<SHEngine::ComputeObject>("Emitter");
	particleEmit_->SetShader("Particle/Test/Emit.CS.hlsl");
	particleEmit_->SetThreadGroupSize(256, 1, 1);
	particleEmit_->SetGPUBuffer(initConfig, BufferType::CBV);
	particleEmit_->SetGPUBuffer(particleNum, BufferType::CBV);
	particleEmit_->SetGPUBuffer(emitConfig, BufferType::CBV);

	particleEmit_->SetGPUBuffer(freeList, BufferType::UAV);
	particleEmit_->SetGPUBuffer(freeListIndex, BufferType::UAV);

	particleEmit_->SetGPUBuffer(positions, BufferType::UAV);
	particleEmit_->SetGPUBuffer(velocities, BufferType::UAV);
	particleEmit_->SetGPUBuffer(lifeTimes, BufferType::UAV);

	particleUpdate_ = std::make_unique<SHEngine::ComputeObject>("Update");
	particleUpdate_->SetShader("Particle/Test/Update.CS.hlsl");
	particleUpdate_->SetThreadGroupSize(256, 1, 1);
	particleUpdate_->SetGPUBuffer(particleNum, BufferType::CBV);
	particleUpdate_->SetGPUBuffer(deadTime, BufferType::CBV);
	particleUpdate_->SetGPUBuffer(deltaTime, BufferType::CBV);
	particleUpdate_->SetGPUBuffer(size, BufferType::CBV);
	particleUpdate_->SetGPUBuffer(camera, BufferType::CBV);

	particleUpdate_->SetGPUBuffer(positions, BufferType::UAV);
	particleUpdate_->SetGPUBuffer(velocities, BufferType::UAV);
	particleUpdate_->SetGPUBuffer(lifeTimes, BufferType::UAV);

	particleUpdate_->SetGPUBuffer(wvp, BufferType::UAV);

	particleUpdate_->SetGPUBuffer(freeList, BufferType::UAV);
	particleUpdate_->SetGPUBuffer(freeListIndex, BufferType::UAV);

	auto drawData = drawDataManager_->GetDrawData(modelManager_->GetNodeModelData(1).drawDataIndex);
	renderer_ = std::make_unique<Renderer>(drawData);
	renderer_->SetVS("Simples.VS.hlsl");
	renderer_->SetPS("White.PS.hlsl");
	renderer_->SetGPUBuffer(wvp, ShaderType::VERTEX_SHADER, BufferType::SRV);
	renderer_->instanceNum_ = kParticleCount;

	commonData_->cmdObject->ResetCommandList();
	particleInit_->Execute(commonData_->cmdObject.get());
}

std::unique_ptr<IScene> TechnicalTestScene::Update() {
	debugCamera_->Update();

	float deltaTime = engine_->GetFPSObserver()->GetDeltatime();

	auto mat = debugCamera_->GetVPMatrix();
	bufferCont_.Copy(1, &mat, sizeof(mat));
	bufferCont_.Copy(6, &deltaTime, sizeof(deltaTime));

	return nullptr;
}

void TechnicalTestScene::Draw() {

	auto window = commonData_->mainWindow.second.get();
	auto display = commonData_->display.get();
	auto cmdObj = commonData_->cmdObject.get();

	particleEmit_->Execute(cmdObj);
	particleUpdate_->Execute(cmdObj);

	display->PreDraw(cmdObj, true);

	renderer_->Draw(cmdObj);

	display->ToPresent(cmdObj);

	window->PreDraw(cmdObj);

#ifdef USE_IMGUI

	ImGui::Begin("Emitter Config");
	bool isInit = bool(emitConfig_.isEmit);
	ImGui::Checkbox("Emit", &isInit);
	emitConfig_.isEmit = int(isInit);
	ImGui::DragInt("Emit Count", &emitConfig_.emitCount, 1.0f, 0);
	ImGui::End();

	ImGui::Begin("Particle Config");
	ImGui::DragFloat("Particle Size", &size_, 0.1f, 0.1f);
	ImGui::DragFloat3("Init Velocity", &initConfig_.velocity.x, 0.1f);
	ImGui::DragFloat3("Init Position", &initConfig_.position.x, 0.1f);
	ImGui::DragFloat("Dead Time", &deadTime_, 0.1f, 0.1f);
	ImGui::End();

	bufferCont_.Copy(2, &initConfig_, sizeof(initConfig_));
	bufferCont_.Copy(3, &size_, sizeof(size_));
	bufferCont_.Copy(4, &emitConfig_, sizeof(emitConfig_));
	bufferCont_.Copy(5, &deadTime_, sizeof(deadTime_));

#endif

	display->DrawImGui();

	engine_->DrawImGui();

	window->ToPresent(cmdObj);

}
