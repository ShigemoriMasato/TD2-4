#include "TestScene.h"
#include <imgui/imgui.h>

namespace {
	bool debug = false;

	struct VertexData {
		Vector4 position;
		Vector2 uv;
		Vector3 normal;
	};
}

void TestScene::Initialize() {
	renderObject_ = std::make_unique<RenderObject>("TestRenderObject");
	renderObject_->Initialize();
	renderObject_->psoConfig_.inputLayoutID = InputLayoutID::Default;
	renderObject_->psoConfig_.vs = "PostEffect/PostEffect.VS.hlsl";
	renderObject_->psoConfig_.ps = "PostEffect/Simple.PS.hlsl";

	std::vector<VertexData> data;
	data.push_back({ {-0.5f, 0.5f, 0.0f, 1.0f}, {0.0f, 0.0f}, {} });
	data.push_back({{0.5f, 0.5f, 0.0f, 1.0f}, {1.0f, 0.0f}, {}});
	data.push_back({{-0.5f, -0.5f, 0.0f, 1.0f}, {0.0f, 1.0f}, {}});
	data.push_back({ { 0.5f, -0.5f, 0.0f, 1.0f }, {1.0f, 1.0f}, {} });

	std::vector<uint32_t> indices = {
		0, 1, 2,
		2, 1, 3
	};
	drawDataManager_->AddVertexBuffer(data);
	drawDataManager_->AddIndexBuffer(indices);

	int drawDataIndex = drawDataManager_->CreateDrawData();
	renderObject_->SetDrawData(drawDataManager_->GetDrawData(drawDataIndex));
	renderObject_->psoConfig_.rootConfig.useTexture = true;
}

std::unique_ptr<IScene> TestScene::Update() {
	ImGui::Begin("Test Scene");
	ImGui::Text("This is a test scene.");
	if (ImGui::Button("Draw")) {
		debug = !debug;
	}
	ImGui::End();

	return nullptr;
}

void TestScene::Draw() {

	auto window = commonData_->mainWindow->GetWindow();

	//dual display -> RenderTarget
	commonData_->display->PreDraw(commonData_->mainWindow->GetCommandList(), true);

	
	renderObject_->Draw(window);
	

	//dyal display -> copy dest
	commonData_->display->PostDraw(commonData_->mainWindow->GetCommandList());


	//swapchain -> RenderTarget
	commonData_->mainWindow->PreDraw();
	//display's Draw(ImGui::Image)
	commonData_->mainWindow->PostDraw();
	//ImGui
	engine_->ImGuiDraw();
}
