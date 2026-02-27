//#include "BackPackBuildScene.h"
//#include <Game/Item/ItemManager.h>
//
//BackPackBuildScene::BackPackBuildScene()
//{
//	itemManager_ = std::make_unique<ItemManager>();
//	camera_ = std::make_unique<DebugCamera>();
//	render_ = std::make_unique<SHEngine::RenderObject>();
//}
//
//BackPackBuildScene::~BackPackBuildScene()
//{}
//
//void BackPackBuildScene::Initialize()
//{
//	int modelHandle = modelManager_->LoadModel("Assets/.EngineResource/Model/Cube");
//	auto modelData = modelManager_->GetNodeModelData(modelHandle);
//	auto drawData = drawDataManager_->GetDrawData(modelData.drawDataIndex);
//
//	itemManager_->Initialize(modelManager_);
//
//	camera_->Initialize(input_);
//
//	render_->Initialize();
//	render_->psoConfig_.vs = "Simple.VS.hlsl"; // WPだけ送るVS
//	render_->psoConfig_.ps = "Simple.PS.hlsl";  // とりあえず白で返すPS
//	render_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER);
//	render_->SetDrawData(drawData);
//}
//
//std::unique_ptr<IScene> BackPackBuildScene::Update()
//{
//	camera_->Update();
//
//	wvp_ = Matrix::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.position);
//	wvp_ *= camera_->GetVPMatrix();
//	render_->CopyBufferData(0, &wvp_, sizeof(Matrix4x4));
//
//	return nullptr;
//}
//
//void BackPackBuildScene::Draw()
//{
//	auto window = commonData_->mainWindow.second.get();
//	auto display = commonData_->display.get();
//	auto cmdObj = commonData_->cmdObject.get();
//
//	display->PreDraw(cmdObj, true);
//	render_->Draw(cmdObj);
//	display->PostDraw(cmdObj);
//
//	window->PreDraw(cmdObj);
//
//
//	//ここ以外で記述する場合、ifdefを忘れないようにすること
//#ifdef USE_IMGUI
//
//	display->DrawImGui();
//
//	ImGui::Begin("FPS");
//	float deltaTime = engine_->GetFPSObserver()->GetDeltatime();
//	ImGui::Text("DeltaTime: %.3f ms", deltaTime * 1000.0f);
//	ImGui::Text("FPS: %.1f", 1.0f / deltaTime);
//	ImGui::End();
//
//	engine_->DrawImGui();
//#endif
//
//	window->PostDraw(cmdObj);
//}
