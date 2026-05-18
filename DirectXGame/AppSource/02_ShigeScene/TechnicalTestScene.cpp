#include "TechnicalTestScene.h"

using namespace SHEngine;

void TechnicalTestScene::Initialize() {
	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize(input_);

	computeCmdObj_ = engine_->CreateCommandObject(Command::Type::Compute, 3);
	particleCmdObj_ = engine_->CreateCommandObject(Command::Type::Direct, 3);

	constexpr uint32_t kParticleCount = 10000000;
	constexpr uint32_t kThreadGroupSize = 1024;

	auto wvp = bufferCont_.Create(BufferType::SRV_UAV, sizeof(Matrix4x4), kParticleCount, 1);
	auto camera = bufferCont_.Create(BufferType::CBV, sizeof(Matrix4x4));

	auto initConfig = bufferCont_.Create(BufferType::CBV, sizeof(InitConfig));
	auto size = bufferCont_.Create(BufferType::CBV, sizeof(float));
	auto emitConfig = bufferCont_.Create(BufferType::CBV, sizeof(EmitConfig));
	auto deltaTime = bufferCont_.Create(BufferType::CBV, sizeof(float));
	auto seed = bufferCont_.Create(BufferType::CBV, sizeof(uint32_t));
	auto particleNum = bufferCont_.Create(BufferType::CBV, sizeof(uint32_t));
	particleNum->CopyBuffer(&kParticleCount, sizeof(kParticleCount));

	auto positions = bufferCont_.Create(BufferType::UAV, sizeof(Vector3), kParticleCount, 1);
	auto velocities = bufferCont_.Create(BufferType::UAV, sizeof(Vector3), kParticleCount, 1);
	auto lifeTimes = bufferCont_.Create(BufferType::UAV, sizeof(float), kParticleCount, 1);

	auto freeList = bufferCont_.Create(BufferType::UAV, sizeof(uint32_t), kParticleCount, 1);
	auto freeListIndex = bufferCont_.Create(BufferType::UAV, sizeof(uint32_t), 1, 1);

	particleInit_ = std::make_unique<SHEngine::ComputeObject>("Initialize");
	particleInit_->SetShader("Particle/Test/Initialize.CS.hlsl");
	particleInit_->SetThreadGroupSize(kParticleCount / kThreadGroupSize, 1, 1);
	particleInit_->SetGPUBuffers(BufferType::UAV, { freeList, freeListIndex, wvp, lifeTimes });
	particleInit_->SetGPUBuffers(BufferType::CBV, { particleNum });

	particleEmit_ = std::make_unique<SHEngine::ComputeObject>("Emitter");
	particleEmit_->SetShader("Particle/Test/Emit.CS.hlsl");
	particleEmit_->SetThreadGroupSize(kParticleCount / kThreadGroupSize, 1, 1);
	particleEmit_->SetGPUBuffers(BufferType::CBV, { initConfig, particleNum, emitConfig, seed });
	particleEmit_->SetGPUBuffers(BufferType::UAV, { freeList, freeListIndex, positions, velocities, lifeTimes });

	particleUpdate_ = std::make_unique<SHEngine::ComputeObject>("Update");
	particleUpdate_->SetShader("Particle/Test/Update.CS.hlsl");
	particleUpdate_->SetThreadGroupSize(kParticleCount / kThreadGroupSize, 1, 1);
	particleUpdate_->SetGPUBuffers(BufferType::CBV, { particleNum, deltaTime, size, camera });
	particleUpdate_->SetGPUBuffers(BufferType::UAV, { positions, velocities, lifeTimes, wvp, freeList, freeListIndex });

	auto drawData = drawDataManager_->GetDrawData(modelManager_->GetNodeModelData(1).drawDataIndex);
	renderer_ = std::make_unique<Renderer>(drawData);
	renderer_->SetVS("Simples.VS.hlsl");
	renderer_->SetPS("White.PS.hlsl");
	renderer_->SetGPUBuffer(wvp, ShaderType::VERTEX_SHADER, BufferType::SRV);
	renderer_->instanceNum_ = kParticleCount;

	computeCmdObj_->ResetCommandList();
	particleInit_->Execute(computeCmdObj_.get());
}

std::unique_ptr<IScene> TechnicalTestScene::Update() {
	debugCamera_->Update();

	float deltaTime = engine_->GetFPSObserver()->GetDeltatime();

	float velX = velDistX_(randomEngine_);
	float velY = velDistY_(randomEngine_);
	float velZ = velDistZ_(randomEngine_);
	initConfig_.velocity = Vector3(velX, velY, velZ).Normalize();

	uint32_t seed = randomEngine_();

	auto mat = debugCamera_->GetVPMatrix();
	bufferCont_.Copy(1, &mat, sizeof(mat));
	bufferCont_.Copy(5, &deltaTime, sizeof(deltaTime));
	bufferCont_.Copy(6, &seed, sizeof(seed));

	return nullptr;
}

void TechnicalTestScene::Draw() {
	auto window = commonData_->mainWindow.second.get();
	auto display = commonData_->display.get();
	auto cmdObj = commonData_->cmdObject.get();
	auto computeCmdObj = computeCmdObj_.get();
	auto particleCmdObj = particleCmdObj_.get();

	computeCmdObj_->ResetCommandList();
	particleCmdObj->ResetCommandList();

	engine_->WaitFence(particleLastFence_, Command::Type::Compute);
	particleEmit_->Execute(computeCmdObj);
	particleUpdate_->Execute(computeCmdObj);
	engine_->ExecuteCommand(Command::Type::Compute, { computeCmdObj });

	display->PreDraw(particleCmdObj, true);

	renderer_->Draw(particleCmdObj);
	particleLastFence_ = engine_->ExecuteCommand(Command::Type::Direct, { particleCmdObj });

	display->PostDraw(cmdObj);

	window->PreDraw(cmdObj);

#ifdef USE_IMGUI

	ImGui::Begin("Emitter Config");
	bool isInit = bool(emitConfig_.isEmit);
	ImGui::Checkbox("Emit", &isInit);
	emitConfig_.isEmit = int(isInit);
	ImGui::DragInt("Emit Count", &emitConfig_.emitCount, 1.0f, 0);
	emitConfig_.emitCount = std::max(emitConfig_.emitCount, 0);
	ImGui::End();

	ImGui::Begin("Particle Config");
	ImGui::DragFloat("Particle Size", &size_, 0.1f, 0.1f);
	ImGui::DragFloat3("Init Velocity", &initConfig_.velocity.x, 0.1f);
	ImGui::DragFloat3("Init Position", &initConfig_.position.x, 0.1f);
	ImGui::DragFloat("Life Time", &initConfig_.lifeTime, 0.1f, 0.1f);
	ImGui::End();

	ImGui::Begin("FPS");
	float deltaTime = engine_->GetFPSObserver()->GetDeltatime();
	ImGui::Text("DeltaTime: %.3f ms", deltaTime * 1000.0f);
	ImGui::Text("FPS: %.1f", 1.0f / deltaTime);
	ImGui::End();

	bufferCont_.Copy(2, &initConfig_, sizeof(initConfig_));
	bufferCont_.Copy(3, &size_, sizeof(size_));
	bufferCont_.Copy(4, &emitConfig_, sizeof(emitConfig_));

#endif

	display->DrawImGui();

	engine_->DrawImGui();

	window->ToPresent(cmdObj);

}
