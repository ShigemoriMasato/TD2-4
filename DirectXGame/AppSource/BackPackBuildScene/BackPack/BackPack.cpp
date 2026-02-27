//#include "BackPack.h"
//
//
//BackPackGrid::BackPackGrid()
//{
//	render_ = std::make_unique<SHEngine::RenderObject>();
//}
//
//BackPackGrid::~BackPackGrid()
//{}
//
//void BackPackGrid::Initialize(SHEngine::ModelManager * modelManager, SHEngine::DrawDataManager* drawDataManager, GridState state)
//{
//	int modelHandle = modelManager->LoadModel("Assets/.EngineResource/Model/Cube");
//	auto modelData = modelManager->GetNodeModelData(modelHandle);
//	auto drawData = drawDataManager->GetDrawData(modelData.drawDataIndex);
//
//	render_->Initialize();
//	render_->psoConfig_.vs = "Simple.VS.hlsl"; // WPだけ送るVS
//	render_->psoConfig_.ps = "Simple.PS.hlsl";  // とりあえず白で返すPS
//	render_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER);
//	render_->SetDrawData(drawData);
//	render_->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER);
//	render_->SetUseTexture(true);
//}
//
//void BackPackGrid::Update()
//{
//}
//
//void BackPackGrid::Draw(SHEngine::Command::Object* cmdObject)
//{
//	if (state_ == GridState::Locked) return;
//	render_->Draw(cmdObject);
//}
//
//
//BackPack::BackPack()
//{}
//
//BackPack::~BackPack()
//{}
//
//void BackPack::Initialize(SHEngine::ModelManager* modelManager)
//{
//
//}