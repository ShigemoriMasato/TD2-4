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

	// === Axe model load ===
	axeModelHandle_ = modelManager_->LoadModel("Assets/Model/Item/Weapon/Axe");
	axeModelData_ = modelManager_->GetNodeModelData(axeModelHandle_);

	// texture index from model data
	auto& material = axeModelData_.materials[axeModelData_.materialIndex.front()];
	axeTextureIndex_ = material.textureIndex;

	axeRender_ = CreateTexturedModelRO(drawDataManager_, axeModelData_, axeTextureIndex_);

	axeTransform_.position = { 0.0f, 0.0f, 0.0f };
	axeTransform_.rotate = { 0.0f, 0.0f, 0.0f };
	axeTransform_.scale = { 1.0f, 1.0f, 1.0f };

	// === Load preset & init trail ===
	// 例：Assets/Json/Axe_Ribbon.json を作ったなら、拡張子なしで "Axe_Ribbon"
	const auto& presetVar = trailPresetRepo_.Get("Axe_Ribbon");

	const auto* ribbonPreset = std::get_if<Ribbon2PointPreset>(&presetVar);
	if (ribbonPreset)
	{
		ribbonTrail_.Initialize(drawDataManager_, textureManager_, *ribbonPreset);
	}
	else
	{
		// typeが違う(JSONがShockwaveRingだった等)
	}
}

std::unique_ptr<IScene> YokoScene::Update()
{
	const float dt = engine_->GetFPSObserver()->GetDeltatime();

	// カメラ更新
	camera_->Update();

	const Matrix4x4 vp = camera_->GetVPMatrix();

	// Axe world/wvp
	const Matrix4x4 world = Matrix::MakeAffineMatrix(axeTransform_.scale, axeTransform_.rotate, axeTransform_.position);
	axeWvp_ = world * vp;

	// model cb update
	const Vector4 color = { 1, 1, 1, 1 };
	axeRender_->CopyBufferData(0, &axeWvp_, sizeof(Matrix4x4));
	axeRender_->CopyBufferData(1, &color, sizeof(Vector4));
	axeRender_->CopyBufferData(2, &axeTextureIndex_, sizeof(int));

	axeTransform_.rotate.y += 0.1f;

	// trail update (model world is required)
	ribbonTrail_.SetModelWorld(world);
	ribbonTrail_.Update(dt, vp);

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

	axeRender_->Draw(cmdObj);
	ribbonTrail_.Draw(cmdObj);

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