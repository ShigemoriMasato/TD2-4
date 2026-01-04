#include "TestScene.h"
#include <imgui/imgui.h>

namespace {
	bool debug = false;

	std::string simpleSkin = "Assets/.EngineResource/Model/SimpleSkin";
	std::string sneekWalk = "SneekWalk";
}

void TestScene::Initialize() {
	renderObject_ = std::make_unique<RenderObject>("TestRenderObject");
	renderObject_->Initialize();
	renderObject_->psoConfig_.inputLayoutID = InputLayoutID::Default;
	renderObject_->psoConfig_.vs = "CameraTest.VS.hlsl";
	renderObject_->psoConfig_.ps = "White.PS.hlsl";

	int LoadModelID = modelManager_->LoadModel(sneekWalk);
	auto model = modelManager_->GetSkinningModelData(LoadModelID);

	renderObject_->SetDrawData(drawDataManager_->GetDrawData(model.drawDataIndex));

	vsDataIndex_ = renderObject_->CreateCBV(sizeof(VSData), ShaderType::VERTEX_SHADER, "TestScene::VSData");

	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize();

	debugLine_ = std::make_unique<DebugLine>();
	debugLine_->Initialize(drawDataManager_, modelManager_, debugCamera_.get());
}

std::unique_ptr<IScene> TestScene::Update() {
	input_->Update();
	debugLine_->Fresh();

	debugCamera_->Update();

	Animation animation = modelManager_->LoadAnimation(sneekWalk, 0);
	auto model = modelManager_->GetSkinningModelData(modelManager_->LoadModel(sneekWalk));

	static float time = 0.0f;
	time += engine_->GetFPSObserver()->GetDeltatime();
	time = std::fmod(time, animation.duration);
	AnimationUpdate(animation, time, model.skeleton);
	SkeletonUpdate(model.skeleton);
	debugLine_->AddLine(model.skeleton, 2.0f);

	return nullptr;
}

void TestScene::Draw() {
	auto window = commonData_->mainWindow.get();
	auto display = commonData_->display.get();

	display->PreDraw(window->GetCommandList(), true);
	renderObject_->Draw(window->GetWindow());
	debugLine_->Draw(window->GetWindow());
	display->PostDraw(window->GetCommandList());

	window->PreDraw();
	window->PostDraw();
	//ImGui
	engine_->ImGuiDraw();
}
