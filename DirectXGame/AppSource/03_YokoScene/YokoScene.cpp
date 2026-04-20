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
	// カメラ初期化
	camera_ = std::make_unique<DebugCamera>();
	camera_->SetProjectionMatrix(PerspectiveFovDesc{});
	camera_->SetPosition({ 0.0f, 3.0f, -10.0f });
	camera_->Initialize(input_);

	// ワールドグリッド初期化
	grid_ = std::make_unique<Grid>();
	grid_->Initialize(drawDataManager_);

	// モデル初期化
	int modelHandle = modelManager_->LoadModel("Assets/Model/Item/Weapon/Axe");
	NodeModelData modelData = modelManager_->GetNodeModelData(modelHandle);
	auto& material = modelData.materials[modelData.materialIndex.front()];
	textureIndex_ = material.textureIndex;
	render_ = CreateTexturedModelRO(drawDataManager_, modelData, textureIndex_);
	transform_.position = { 0.0f, 0.0f, 0.0f };
	transform_.rotate = { 0.0f, 0.0f, 0.0f };
	transform_.scale = { 1.0f, 1.0f, 1.0f };

	// トレイル初期化
	trail.Initialize(textureManager_, commonData_);
	trail.Add("testTrail2");
	trail.Add("testTrail2_1");

	// パーティクル初期化
	particles_.Initialize(textureManager_, modelManager_, commonData_);
	particles_.Add("sparrrrk2");

	// TrailOnParticle初期化
	TrailOnParticle.Initialize(textureManager_, modelManager_, commonData_, "death", "sparrrk");
}

std::unique_ptr<IScene> YokoScene::Update()
{
	const float dt = engine_->GetFPSObserver()->GetDeltatime();
	const bool isSpace = input_->GetKeyState(DIK_SPACE);
	const bool isSpaceTrigger = isSpace && !input_->GetPreKeyState(DIK_SPACE);

	// カメラ更新
	camera_->Update();
	const Matrix4x4 vp = camera_->GetVPMatrix();

	// ワールドグリッド更新
	grid_->Update(Vector3(0.0f, 0.0f, 0.0f), vp);

	if (isSpaceTrigger)
	{
		transform_.scale = { 1.0f, 1.0f, 1.0f };
		transform_.position = { 0.0f, -2.0f, 0.0f };
		transform_.rotate = { 0.0f, 0.0f, 0.0f };

		trail.Clear();

		TrailOnParticle.Trigger();
		start = true;
	}
	if (start)
	{
		transform_.position.y += 0.06f;
		transform_.rotate.y += 0.4f;

		if (transform_.position.y < 0.0f)
		{
			transform_.scale.x += 0.04f;
			transform_.scale.y += 0.04f;
			transform_.scale.z += 0.04f;
		}
		else if (transform_.position.y >= 0.0f)
		{
			transform_.scale.x -= 0.04f;
			transform_.scale.y -= 0.04f;
			transform_.scale.z -= 0.04f;
		}

		if (transform_.position.y > 2.0f)
		{
			start = false;
		}
	}

	// モデル更新
	const Matrix4x4 world = Matrix::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.position);
	const Matrix4x4 wvp = world * vp;
	const Vector4 color = { 1, 1, 1, 1 };
	render_->CopyBufferData(0, &wvp, sizeof(Matrix4x4));
	render_->CopyBufferData(1, &color, sizeof(Vector4));
	render_->CopyBufferData(2, &textureIndex_, sizeof(int));

	// TrailOnParticle更新
	TrailOnParticle.SetModelWorld(Matrix4x4::Identity());
	TrailOnParticle.Update(dt);


	// トレイル更新
	trail.SetModelWorld(world);
	trail.Update(dt);

	// パーティクル更新
	//particles_.SetModelWorld(world);
	//particles_.Update(dt);

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

	grid_->Draw(cmdObj);

	// TrailOnParticle描画

	TrailOnParticle.Draw();

	trail.Draw();
	particles_.Draw();
	



	// パーティクル描画
	commonData_->particleDrawer->Draw(cmdObj, camera_->GetVPMatrix());

	// トレイル描画
	commonData_->trailDrawer->Draw(cmdObj, camera_->GetVPMatrix());

	display->ToPresent(cmdObj);

	window->PreDraw(cmdObj);

#ifdef USE_IMGUI
	display->DrawImGui();

	ImGui::Begin("Axe Transform");
	ImGui::DragFloat3("T", &transform_.position.x, 0.1f);
	ImGui::DragFloat3("R", &transform_.rotate.x, 0.1f);
	ImGui::DragFloat3("S", &transform_.scale.x, 0.1f);
	ImGui::End();


	ImGui::Begin("FPS");
	float deltaTime = engine_->GetFPSObserver()->GetDeltatime();
	ImGui::Text("DeltaTime: %.3f ms", deltaTime * 1000.0f);
	ImGui::Text("FPS: %.1f", 1.0f / deltaTime);
	ImGui::End();
#endif

	engine_->DrawImGui();
	window->ToPresent(cmdObj);
}