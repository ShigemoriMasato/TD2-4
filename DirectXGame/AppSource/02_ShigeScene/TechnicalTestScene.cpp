#include "TechnicalTestScene.h"

using namespace SHEngine;

void TechnicalTestScene::Initialize() {
	DrawData data = drawDataManager_->GetDrawData(modelManager_->GetNodeModelData(1).drawDataIndex);
	text_ = std::make_unique<Text>();
	text_->Initialize(data, "YDWbananaslipplus.otf", 196);
	text_->SetText(L"Hello, World!");

	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize(input_);
	text_->SetColor({ 1.0f, 1.0f, 0.0f, 1.0f });

	int sneekWalkModelID = modelManager_->LoadModel("Assets/Model/SneekWalk");
	skinningModelData_ = &modelManager_->GetSkinningModelData(sneekWalkModelID);
	animation_ = modelManager_->LoadAnimation("Assets/Model/SneekWalk", 0);
	skinningPalette_.resize(skinningModelData_->skeleton.joints.size());

	auto drawData = drawDataManager_->GetDrawData(skinningModelData_->drawDataIndex);
	renderer_ = std::make_unique<Renderer>(drawData);
	renderer_->vs_ = "Skinning.VS.hlsl";
	renderer_->ps_ = "PostEffect/Simple.PS.hlsl";
	renderer_->inputLayoutID_ = PSO::InputLayoutID::Skinning;
	renderer_->isUseTexture_ = true;

	ResourceDesc desc{};
	gpuBuffers_.reserve(10);

	desc.bufferType = uint8_t(BufferType::CBV);
	desc.elementCount = 1;
	desc.sizeInBytes = sizeof(SkinningTransformMatrix);
	auto& transform = gpuBuffers_.emplace_back(std::make_unique<GPUBuffer>(desc));

	desc.sizeInBytes = sizeof(int);
	auto& textureIndex = gpuBuffers_.emplace_back(std::make_unique<GPUBuffer>(desc));

	desc.bufferType = uint8_t(BufferType::SRV);
	desc.elementCount = uint32_t(skinningPalette_.size());
	desc.sizeInBytes = sizeof(WellForGPU);
	auto& matrixPalette = gpuBuffers_.emplace_back(std::make_unique<GPUBuffer>(desc));

	desc.bufferType = uint8_t(BufferType::CBV);
	desc.elementCount = 1;
	desc.sizeInBytes = sizeof(SkinningInformation);
	auto& skinningInfo = gpuBuffers_.emplace_back(std::make_unique<GPUBuffer>(desc));

	desc.bufferType = uint8_t(BufferType::SRV);
	desc.elementCount = uint32_t(skinningModelData_->vertices.size());
	desc.sizeInBytes = sizeof(VertexData);
	auto& vertices = gpuBuffers_.emplace_back(std::make_unique<GPUBuffer>(desc));

	desc.bufferType = uint8_t(BufferType::SRV);
	desc.elementCount = uint32_t(skinningModelData_->vertexInfluences.size());
	desc.sizeInBytes = sizeof(VertexInfluence);
	auto& influences = gpuBuffers_.emplace_back(std::make_unique<GPUBuffer>(desc));

	desc.bufferType = uint8_t(BufferType::UAV | BufferType::SRV);
	desc.initialState = D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
	desc.elementCount = uint32_t(skinningModelData_->vertices.size());
	desc.sizeInBytes = sizeof(Matrix4x4);
	auto& output = gpuBuffers_.emplace_back(std::make_unique<GPUBuffer>(desc));

	desc.bufferType = uint8_t(BufferType::CBV);
	desc.elementCount = 1;
	desc.sizeInBytes = sizeof(Matrix4x4);
	auto& cameraBuffer = gpuBuffers_.emplace_back(std::make_unique<GPUBuffer>(desc));

	renderer_->SetGPUBuffer(textureIndex.get(), ShaderType::PIXEL_SHADER, BufferType::CBV);
	renderer_->SetGPUBuffer(output.get(), ShaderType::VERTEX_SHADER, BufferType::SRV);
	renderer_->SetGPUBuffer(cameraBuffer.get(), ShaderType::VERTEX_SHADER, BufferType::CBV);

	skinningCompute_ = std::make_unique<ComputeObject>("TechnicalTestSceneSkinning");
	skinningCompute_->SetShader("Skinning.CS.hlsl");
	skinningCompute_->SetThreadGroupSize((int(skinningModelData_->vertices.size()) + 1023) / 1024);
	skinningCompute_->SetGPUBuffer(skinningInfo.get(), BufferType::CBV, ShaderType::COMPUTE_SHADER);
	skinningCompute_->SetGPUBuffer(matrixPalette.get(), BufferType::SRV, ShaderType::COMPUTE_SHADER);
	skinningCompute_->SetGPUBuffer(vertices.get(), BufferType::SRV, ShaderType::COMPUTE_SHADER);
	skinningCompute_->SetGPUBuffer(influences.get(), BufferType::SRV, ShaderType::COMPUTE_SHADER);
	skinningCompute_->SetGPUBuffer(output.get(), BufferType::UAV, ShaderType::COMPUTE_SHADER);

	skinningInfo_.numVertices = uint32_t(skinningModelData_->vertices.size());
	gpuBuffers_[kBufferCSInfo]->CopyBuffer(&skinningInfo_, sizeof(SkinningInformation));
	gpuBuffers_[kBufferCSVertices]->CopyBuffer(skinningModelData_->vertices.data(), sizeof(VertexData) * skinningModelData_->vertices.size());
	gpuBuffers_[kBufferCSInfluences]->CopyBuffer(skinningModelData_->vertexInfluences.data(), sizeof(VertexInfluence) * skinningModelData_->vertexInfluences.size());

	int textureID = skinningModelData_->materials[skinningModelData_->materialIndex.front()].textureIndex;
	if (textureID < 0) {
		textureID = 0;
	}
	gpuBuffers_[kBufferTextureIndex]->CopyBuffer(&textureID, sizeof(int));

	computeCmdObj_ = engine_->CreateCommandObject(SHEngine::Command::Type::Compute, 0, 1);
	computeCmdObj_->ResetCommandList();
}

std::unique_ptr<IScene> TechnicalTestScene::Update() {
	debugCamera_->Update();
	text_->Update(debugCamera_->GetVPMatrix());
	text_->SetTransform(textTransform_);

	if (animation_.duration > 0.0f) {
		animationTime_ += 1.0f / 60.0f;
		while (animationTime_ > animation_.duration) {
			animationTime_ -= animation_.duration;
		}
	}

	AnimationUpdate(animation_, animationTime_, skinningModelData_->skeleton);
	SkeletonUpdate(skinningModelData_->skeleton);
	SkinningUpdate(skinningPalette_, skinningModelData_->skinClusterData, skinningModelData_->skeleton);

	skinningTransform_.world = Matrix4x4::Identity();
	skinningTransform_.vp = debugCamera_->GetVPMatrix();
	gpuBuffers_[kBufferTransform]->CopyBuffer(&skinningTransform_, sizeof(SkinningTransformMatrix));
	gpuBuffers_[kBufferMatrixPalette]->CopyBuffer(skinningPalette_.data(), sizeof(WellForGPU) * skinningPalette_.size());
	Matrix4x4 vpMat = debugCamera_->GetVPMatrix();
	gpuBuffers_.back()->CopyBuffer(&vpMat, sizeof(vpMat));

	engine_->ExecuteCommand(SHEngine::Command::Type::Compute, 0, { computeCmdObj_.get() });

	return nullptr;
}

void TechnicalTestScene::Draw() {

	auto window = commonData_->mainWindow.second.get();
	auto display = commonData_->display.get();
	auto cmdObj = commonData_->cmdObject.get();

	skinningCompute_->Execute(cmdObj);

	display->PreDraw(cmdObj, true);

	skinningCompute_->Execute(cmdObj);
	text_->Draw(cmdObj);
	renderer_->Draw(cmdObj);

	display->PostDraw(cmdObj);

	window->PreDraw(cmdObj);

#ifdef USE_IMGUI

	ImGui::Begin("Text");
	ImGui::DragFloat3("Scale", &textTransform_.scale.x, 0.01f);
	ImGui::DragFloat3("Rotate", &textTransform_.rotate.x, 0.01f);
	ImGui::DragFloat3("Position", &textTransform_.position.x, 0.01f);
	ImGui::End();

	ImGui::Begin("FPS");
	float deltaTime = engine_->GetFPSObserver()->GetDeltatime();
	ImGui::Text("DeltaTime: %.3f ms", deltaTime * 1000.0f);
	ImGui::Text("FPS: %.1f", 1.0f / deltaTime);
	ImGui::End();

#endif

	display->DrawImGui();

	engine_->DrawImGui();

	window->PostDraw(cmdObj);

	//描画を実行する前にcomputeShaderがバッファを更新するまで待つ
	computeCmdObj_->ResetCommandList();
}
