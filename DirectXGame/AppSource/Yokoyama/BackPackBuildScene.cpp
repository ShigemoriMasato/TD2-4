#include "BackPackBuildScene.h"
#include <Game/Item/ItemManager.h>

BackPackBuildScene::BackPackBuildScene()
{
	itemManager_ = std::make_unique<ItemManager>();

	//インスタンスの作成
	std::unique_ptr<SHEngine::RenderObject> renderObject = std::make_unique<SHEngine::RenderObject>("DebugName");
	renderObject->Initialize();

	//Shaderの設定
	renderObject->psoConfig_.vs = "DebugVS.hlsl";
	renderObject->psoConfig_.ps = "DebugPS.hlsl";

	//CBV,SRVの設定
	int colorIndex = renderObject->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "DebugName");
	int textureIndex = renderObject->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "DebugName");
	int wvpIndex = renderObject->CreateSRV(sizeof(Matrix4x4), 100, ShaderType::VERTEX_SHADER, "DebugName");

	//Textureを使用するかどうか
	renderObject->SetUseTexture(true);
}

BackPackBuildScene::~BackPackBuildScene()
{}

void BackPackBuildScene::Initialize()
{
	itemManager_->Initialize(modelManager_);
}

std::unique_ptr<IScene> BackPackBuildScene::Update()
{

	Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
	renderObject->CopyBufferData(colorIndex, &color, sizeof(Vector4));

	int texture = 0;
	renderObject->CopyBufferData(textureIndex, &texture, sizeof(int));

	std::vector<Matrix4x4> wvp{};
	renderObject->CopyBufferData(wvpIndex, wvp.data(), sizeof(Matrix4x4) * wvp.size());


	return nullptr;
}

void BackPackBuildScene::Draw()
{
	auto window = commonData_->mainWindow.second.get();
	auto display = commonData_->display.get();
	auto cmdObj = commonData_->cmdObject.get();

	display->PreDraw(cmdObj, true);
	display->PostDraw(cmdObj);

	window->PreDraw(cmdObj);

	CmdObj* cmdObj = commonData_->cmdObject.get();
	renderObject->Draw(cmdObj);

	//ここ以外で記述する場合、ifdefを忘れないようにすること
#ifdef USE_IMGUI

	display->DrawImGui();

	itemManager_->DrawImGui();

	ImGui::Begin("FPS");
	float deltaTime = engine_->GetFPSObserver()->GetDeltatime();
	ImGui::Text("DeltaTime: %.3f ms", deltaTime * 1000.0f);
	ImGui::Text("FPS: %.1f", 1.0f / deltaTime);
	ImGui::End();

	engine_->DrawImGui();
#endif

	window->PostDraw(cmdObj);
}
