#include "YokoScene.h"
#include <numbers>
#include <algorithm>
#include "03_YokoScene/TrailEditorScene.h"

namespace
{
	std::unique_ptr<SHEngine::RenderObject> CreateTexturedModelRO(
		SHEngine::DrawDataManager* drawDataManager,
		const NodeModelData& modelData,
		int textureIndex)
	{
		auto ro = std::make_unique<SHEngine::RenderObject>();
		ro->Initialize();

		ro->psoConfig_.vs = "Game/Field.VS.hlsl";
		ro->psoConfig_.ps = "Game/Field.PS.hlsl";
		ro->SetUseTexture(true);

		ro->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER);
		ro->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "Color");
		ro->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");

		const auto drawData = drawDataManager->GetDrawData(modelData.drawDataIndex);
		ro->SetDrawData(drawData);

		const Vector4 color = { 1, 1, 1, 1 };
		ro->CopyBufferData(1, &color, sizeof(Vector4));
		ro->CopyBufferData(2, &textureIndex, sizeof(int));

		return ro;
	}
}

void YokoScene::Initialize()
{
	camera_ = std::make_unique<DebugCamera>();
	camera_->SetProjectionMatrix(PerspectiveFovDesc{});
	camera_->SetPosition({ 0.0f, 8.0f, -25.0f });
	camera_->Initialize(input_);

	modelHandle_ = modelManager_->LoadModel("Assets/Model/Item/Weapon/Axe");
	modelData_ = modelManager_->GetNodeModelData(modelHandle_);
	auto& material = modelData_.materials[modelData_.materialIndex.front()];
	textureIndex_ = material.textureIndex;

	render_ = CreateTexturedModelRO(drawDataManager_, modelData_, textureIndex_);

	transform_.position = { 0.0f, 0.0f, 0.0f };
	transform_.rotate = { 0.0f, 0.0f, 0.0f };
	transform_.scale = { 1.0f, 1.0f, 1.0f };

	const auto& preset1 = trailPresetRepo_.Get("Axe_Ribbon");
	const auto& preset2 = trailPresetRepo_.Get("Axe_Ribbon2");

	if (std::holds_alternative<RibbonTrailPreset>(preset1))
	{
		ribbonTrail1_.Initialize(drawDataManager_, textureManager_, std::get<RibbonTrailPreset>(preset1));
	}
	if (std::holds_alternative<RibbonTrailPreset>(preset2))

	{
		ribbonTrail2_.Initialize(drawDataManager_, textureManager_, std::get<RibbonTrailPreset>(preset2));
	}
}

std::unique_ptr<IScene> YokoScene::Update()
{
	const float dt = engine_->GetFPSObserver()->GetDeltatime();

	// カメラ更新
	camera_->Update();
	const Matrix4x4 vp = camera_->GetVPMatrix();

	// モデル更新
	const Matrix4x4 world = Matrix::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.position);
	wvp_ = world * vp;
	const Vector4 color = { 1, 1, 1, 1 };
	render_->CopyBufferData(0, &wvp_, sizeof(Matrix4x4));
	render_->CopyBufferData(1, &color, sizeof(Vector4));
	render_->CopyBufferData(2, &textureIndex_, sizeof(int));


	// trail update (model world is required)
	ribbonTrail1_.SetModelWorld(world);
	ribbonTrail1_.Update(dt, vp);
	ribbonTrail2_.SetModelWorld(world);
	ribbonTrail2_.Update(dt, vp);

	// Zキーで決定
	if (input_->GetKeyState(DIK_Z) && !input_->GetPreKeyState(DIK_Z))
	{
		return std::make_unique<TrailEditorScene>();
	}

	return nullptr;
}

void YokoScene::Draw()
{
	auto window = commonData_->mainWindow.second.get();
	auto display = commonData_->display.get();
	auto cmdObj = commonData_->cmdObject.get();

	display->PreDraw(cmdObj, true);

	render_->Draw(cmdObj);
	ribbonTrail1_.Draw(cmdObj);
	ribbonTrail2_.Draw(cmdObj);

	display->PostDraw(cmdObj);

	window->PreDraw(cmdObj);

#ifdef USE_IMGUI
	display->DrawImGui();

	ImGui::Begin("Axe Transform");
	ImGui::DragFloat3("T", &transform_.position.x, 0.1f);
	ImGui::DragFloat3("R", &transform_.rotate.x, 0.1f);
	ImGui::DragFloat3("S", &transform_.scale.x, 0.1f);
	ImGui::End();
#endif

	engine_->DrawImGui();
	window->PostDraw(cmdObj);
}