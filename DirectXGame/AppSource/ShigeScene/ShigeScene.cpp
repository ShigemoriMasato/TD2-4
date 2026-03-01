#include "ShigeScene.h"
#include <imgui/imgui.h>
#include <Utility/Color.h>

void ShigeScene::Initialize() {
	camera_ = std::make_unique<DebugCamera>();
	camera_->Initialize(input_);

	grid_ = std::make_unique<Grid>();
	grid_->Initialize(drawDataManager_);

	computeObject_ = std::make_unique<SHEngine::ComputeObject>("TestCompute");
	computeObject_->Initialize();

	sneekWalk_ = modelManager_->GetSkinningModelData(modelManager_->LoadModel("SneekWalk"));
	animation_ = modelManager_->LoadAnimation("SneekWalk", 0);

	computeObject_->CreateSRV(sizeof(WellForGPU), int(sneekWalk_.skeleton.joints.size()));
	computeObject_->CreateSRV(sizeof(VertexData), int(sneekWalk_.vertices.size()));
	computeObject_->CreateSRV(sizeof(VertexInfluence), int(sneekWalk_.vertexInfluences.size()));
	computeObject_->CreateCBV(sizeof(uint32_t));
	computeObject_->CreateUAV(sizeof(VertexData), int(sneekWalk_.vertices.size()));
	computeObject_->SetShader("Skinning.CS.hlsl");
	computeObject_->SetThreadGroupSize(int(sneekWalk_.vertices.size()));

	drawDataManager_->AddVertexBuffer(sneekWalk_.vertices);
	drawDataManager_->AddIndexBuffer(sneekWalk_.indices);
	int drawDataIndex = drawDataManager_->CreateDrawData();
	auto drawData = drawDataManager_->GetDrawData(drawDataIndex);
	sneekWalk_.drawDataIndex = drawDataIndex;

	render_ = std::make_unique<SHEngine::RenderObject>("TestRender");
	render_->Initialize();
	render_->SetDrawData(drawData);
	render_->psoConfig_.vs = "Simple.VS.hlsl";
	render_->psoConfig_.ps = "White.PS.hlsl";
	render_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "ObjectCBV");

	computeCmdObj_ = engine_->CreateCommandObject(SHEngine::Command::Type::Compute, 0, 1);

	skinnedVertices_.resize(sneekWalk_.vertices.size());
}

std::unique_ptr<IScene> ShigeScene::Update() {
	camera_->Update();
	Vector3 cameraPos = camera_->GetCenter();
	grid_->Update(cameraPos, camera_->GetVPMatrix());

	Matrix4x4 world = Matrix4x4::Identity() * camera_->GetVPMatrix();
	render_->CopyBufferData(0, &world, sizeof(world));

	float deltaTime = engine_->GetDeltaTime();
	time_ = std::fmod(time_ + deltaTime, animation_.duration);
	AnimationUpdate(animation_, time_, sneekWalk_.skeleton);
	SkeletonUpdate(sneekWalk_.skeleton);
	SkinningUpdate(well_, sneekWalk_.skinClusterData, sneekWalk_.skeleton);

	computeObject_->CopyBufferData(0, well_.data(), sizeof(WellForGPU) * well_.size());
	computeObject_->CopyBufferData(1, sneekWalk_.vertices.data(), sizeof(VertexData) * sneekWalk_.vertices.size());
	computeObject_->CopyBufferData(2, sneekWalk_.vertexInfluences.data(), sizeof(VertexInfluence) * sneekWalk_.vertexInfluences.size());
	uint32_t vertexNum = uint32_t(sneekWalk_.vertices.size());
	computeObject_->CopyBufferData(3, &vertexNum, sizeof(vertexNum));

	computeCmdObj_->ResetCommandList();
	computeObject_->Execute(computeCmdObj_.get());
	engine_->ExecuteCommand(SHEngine::Command::Type::Compute, 0);
	computeCmdObj_->WaitForCanExecute();

	computeObject_->GetUAVBuffer(0, skinnedVertices_.data(), sizeof(VertexData) * skinnedVertices_.size());
	drawDataManager_->CopyBufferData(sneekWalk_.drawDataIndex, skinnedVertices_.data(), sizeof(VertexData) * sneekWalk_.vertices.size());

	return nullptr;
}

void ShigeScene::Draw() {
	auto window = commonData_->mainWindow.second.get();
	auto display = commonData_->display.get();
	auto cmdObj = commonData_->cmdObject.get();

	display->PreDraw(cmdObj, true);

	grid_->Draw(cmdObj);
	render_->Draw(cmdObj);

	display->PostDraw(cmdObj);

	window->PreDraw(cmdObj);

	//ここ以外で記述する場合、ifdefを忘れないようにすること
#ifdef USE_IMGUI


	display->DrawImGui();

	ImGui::Begin("FPS");
	float deltaTime = engine_->GetFPSObserver()->GetDeltatime();
	ImGui::Text("DeltaTime: %.3f ms", deltaTime * 1000.0f);
	ImGui::Text("FPS: %.1f", 1.0f / deltaTime);
	ImGui::End();
#endif


	engine_->DrawImGui();
	window->PostDraw(cmdObj);
}
