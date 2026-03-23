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

	int modelHandle = modelManager_->LoadModel("Assets/Model/Item/Weapon/Axe");
	NodeModelData modelData = modelManager_->GetNodeModelData(modelHandle);
	auto& material = modelData.materials[modelData.materialIndex.front()];
	textureIndex_ = material.textureIndex;
	render_ = CreateTexturedModelRO(drawDataManager_, modelData, textureIndex_);

	axeTransform_.position = { 0.0f, 0.0f, 0.0f };
	axeTransform_.rotate = { 0.0f, 0.0f, 0.0f };
	axeTransform_.scale = { 1.0f, 1.0f, 1.0f };

	trail_Axe.Initialize(drawDataManager_, textureManager_, &trailDataBank_);
	trail_Axe.Add("Axe_Ribbon");
	trail_Axe.Add("Axe_Ribbon2");

	trail_test1.Initialize(drawDataManager_, textureManager_, &trailDataBank_);
	trail_test1.Add("testTrail1_1");
	trail_test1.Add("testTrail1_2");
	trail_test1.Add("testTrail1_3");
	trail_test1.Add("testTrail1_4");

}

std::unique_ptr<IScene> YokoScene::Update()
{
	const float dt = engine_->GetFPSObserver()->GetDeltatime();

	// カメラ更新
	camera_->Update();
	const Matrix4x4 vp = camera_->GetVPMatrix();

	// モデル更新
	const Matrix4x4 world = Matrix::MakeAffineMatrix(axeTransform_.scale, axeTransform_.rotate, axeTransform_.position);
	const Matrix4x4 wvp = world * vp;
	const Vector4 color = { 1, 1, 1, 1 };
	render_->CopyBufferData(0, &wvp, sizeof(Matrix4x4));
	render_->CopyBufferData(1, &color, sizeof(Vector4));
	render_->CopyBufferData(2, &textureIndex_, sizeof(int));


	// トレイル更新
	trail_Axe.SetModelWorld(world);
	trail_Axe.Update(dt, vp);
	trail_test1.SetModelWorld(world);
	trail_test1.Update(dt, vp);

	// Zキーでエディタ切り替え
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
	trail_Axe.Draw(cmdObj);
	trail_test1.Draw(cmdObj);

	display->PostDraw(cmdObj);

	window->PreDraw(cmdObj);

#ifdef USE_IMGUI
	display->DrawImGui();

	ImGui::Begin("Axe Transform");
	ImGui::DragFloat3("T", &axeTransform_.position.x, 0.1f);
	ImGui::DragFloat3("R", &axeTransform_.rotate.x, 0.1f);
	ImGui::DragFloat3("S", &axeTransform_.scale.x, 0.1f);
	ImGui::End();
#endif

	engine_->DrawImGui();
	window->PostDraw(cmdObj);
}