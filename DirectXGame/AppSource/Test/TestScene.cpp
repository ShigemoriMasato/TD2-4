#include "TestScene.h"
#include <imgui/imgui.h>
#include <Utility/ConvertString.h>
#include"Common/DebugParam/GameParamEditor.h"
#include"FpsCount.h"
#include <Utility/Easing.h>

namespace {
	bool debug = false;

	std::string simpleSkin = "Assets/.EngineResource/Model/SimpleSkin";
	std::string sneakWalk = "SneakWalk";
}

void TestScene::Initialize() {
	// 登録するパラメータを設定
	GameParamEditor::GetInstance()->SetActiveScene("TestScene");
	// パラメーター管理の初期化
	paramManager_ = std::make_unique<ParamManager>();

	// テスト用パラメータを登録
	GameParamEditor::GetInstance()->AddItem("TestGroup", "TestParam", testParam_);
	// 適応
	testParam_ = GameParamEditor::GetInstance()->GetValue<float>("TestGroup", "TestParam");

	renderObject_ = std::make_unique<RenderObject>("TestRenderObject");
	renderObject_->Initialize();
	renderObject_->psoConfig_.inputLayoutID = InputLayoutID::Skinning;
	renderObject_->psoConfig_.vs = "Skinning.VS.hlsl";
	renderObject_->psoConfig_.ps = "White.PS.hlsl";

	int LoadModelID = modelManager_->LoadModel(sneakWalk);
	auto model = modelManager_->GetSkinningModelData(LoadModelID);

	renderObject_->SetDrawData(drawDataManager_->GetDrawData(model.drawDataIndex));

	vsDataIndex_ = renderObject_->CreateCBV(sizeof(VSData), ShaderType::VERTEX_SHADER, "TestScene::VSData");
	int jointCount = static_cast<int>(model.skeleton.joints.size());
	skinningMatrices_.resize(jointCount);
	renderObject_->CreateSRV(sizeof(WellForGPU), jointCount, ShaderType::VERTEX_SHADER, "TestScene::SkinningMatrices");

	debugCamera_ = std::make_unique<DebugCamera>();
	debugCamera_->Initialize();

	debugLine_ = std::make_unique<DebugLine>();
	debugLine_->Initialize(drawDataManager_, modelManager_, debugCamera_.get());

	fontLoader_->Load(fontName);

	fontTest_ = std::make_unique<RenderObject>("FontTest");
	fontTest_->Initialize();
	fontTest_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "FontTest::VSData");
	fontTest_->CreateSRV(sizeof(CharPosition), 256, ShaderType::VERTEX_SHADER, "FontTest::CharPosition");
	fontTest_->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "FontTest::TextureIndex");
	fontTest_->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "FontTest::FontColor");
	auto drawData = drawDataManager_->GetDrawData(modelManager_->GetNodeModelData(1).drawDataIndex);
	fontTest_->SetDrawData(drawData);
	fontTest_->SetUseTexture(true);
	fontTest_->psoConfig_.vs = "Font/FontBasic.VS.hlsl";
	fontTest_->psoConfig_.ps = "Font/FontBasic.PS.hlsl";
	charPositions_.reserve(256);

	orthoCamera_ = std::make_unique<Camera>();
	orthoCamera_->SetProjectionMatrix(OrthographicDesc());
	orthoCamera_->MakeMatrix();

	for(int i = 0; i < int(TileType::Count); ++i){
		Vector4 color = ConvertColor(lerpColor(0xff6600ff, 0x0066ffff, float(i) / (float(TileType::Count) - 1)));
		colorMap_[static_cast<TileType>(i)] = Vector3(color.x, color.y, color.z);
	}

	currentMap_ = commonData_->newMapManager->GetMapData(0);
	currentMap_.Rotate(Direction::Back);

	mapRender_ = std::make_unique<MapRender>();
	drawData = drawDataManager_->GetDrawData(modelManager_->GetNodeModelData(0).drawDataIndex);
	mapRender_->Initialize(drawData, 0);
	mapRender_->SetConfig(currentMap_.renderData);
	debugMCRender_ = std::make_unique<DebugMCRender>();
	debugMCRender_->Initialize(drawData);
	debugMCRender_->SetAlpha(0.4f);

	modelTest_ = std::make_unique<RenderObject>("ModelTest");
	modelTest_->Initialize();
	LoadModelID = modelManager_->LoadModel("VisionFrame");
	drawData = drawDataManager_->GetDrawData(modelManager_->GetNodeModelData(LoadModelID).drawDataIndex);
	modelTest_->SetDrawData(drawData);
	modelTest_->CreateCBV(sizeof(Matrix4x4), ShaderType::VERTEX_SHADER, "WVP");
	modelTest_->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "TextureIndex");
	modelTest_->SetUseTexture(true);
	modelTest_->psoConfig_.vs = "Simple.VS.hlsl";
	modelTest_->psoConfig_.ps = "PostEffect/Simple.PS.hlsl";
}

std::unique_ptr<IScene> TestScene::Update() {
#ifdef USE_IMGUI
	// 値の適応
	testParam_ = GameParamEditor::GetInstance()->GetValue<float>("TestGroup", "TestParam");
#endif

	// Δタイムを取得する
	FpsCount::deltaTime = engine_->GetFPSObserver()->GetDeltatime();

	commonData_->keyManager->Update();
	input_->Update();
	debugLine_->Fresh();

	debugCamera_->Update();

	static Animation animation = modelManager_->LoadAnimation(sneakWalk, 0);
	static auto model = modelManager_->GetSkinningModelData(modelManager_->LoadModel(sneakWalk));

	static float time = 0.0f;
	time += engine_->GetFPSObserver()->GetDeltatime();
	time = std::fmod(time, animation.duration);

	AnimationUpdate(animation, time, model.skeleton);
	SkeletonUpdate(model.skeleton);
	SkinningUpdate(skinningMatrices_, model.skinClusterData, model.skeleton);

	vsData_.worldMatrix = Matrix4x4::Identity();
	vsData_.vpMatrix = debugCamera_->GetVPMatrix();

	renderObject_->CopyBufferData(vsDataIndex_, &vsData_, sizeof(VSData));
	renderObject_->CopyBufferData(1, skinningMatrices_.data(), sizeof(WellForGPU) * skinningMatrices_.size());

	debugLine_->AddLine(model.skeleton, 2.0f);

	charPositions_.clear();
	for(const wchar_t& c : text_) {
		CharPosition charPos = fontLoader_->GetCharPosition(fontName, c, 64);
		charPositions_.push_back(charPos);
	}

	return nullptr;
}

void TestScene::Draw() {
	auto window = commonData_->mainWindow.get();
	auto display = commonData_->display.get();

	Matrix4x4 vpMatrix3d = debugCamera_->GetVPMatrix();

	display->PreDraw(window->GetCommandObject(), true);
	//renderObject_->Draw(window->GetWindow());
	debugLine_->Draw(window->GetWindow());

	int textureIndex = fontLoader_->Load(fontName);
	wvpMat_ = Matrix::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.position) * orthoCamera_->GetVPMatrix();
	fontTest_->CopyBufferData(0, &wvpMat_, sizeof(Matrix4x4));
	fontTest_->CopyBufferData(1, charPositions_.data(), sizeof(CharPosition) * charPositions_.size());
	fontTest_->CopyBufferData(2, &textureIndex, sizeof(int));
	fontTest_->CopyBufferData(3, &fontColor_, sizeof(Vector4));
	fontTest_->instanceNum_ = static_cast<uint32_t>(charPositions_.size());
	fontTest_->Draw(window->GetWindow());

	modelTest_->CopyBufferData(0, &vpMatrix3d, sizeof(Matrix4x4));
	textureIndex = textureManager_->LoadTexture("Mineral-0.png");
	modelTest_->CopyBufferData(1, &textureIndex, sizeof(int));
	modelTest_->Draw(window->GetWindow());

	mapRender_->Draw(debugCamera_->GetVPMatrix(), window->GetWindow());
	debugMCRender_->Draw(debugCamera_->GetVPMatrix(), colorMap_,  currentMap_.mapChipData, window->GetWindow());

	display->PostDraw(window->GetCommandObject());

	window->PreDraw(true);
	window->DrawDisplayWithImGui();
	//ImGui

#ifdef USE_IMGUI

	ImGui::Begin("FontTest");
	ImGui::ColorEdit4("FontColor", &fontColor_.x);
	ImGui::InputText("InputText", willLoadPath_, 256);
	text_ = ConvertString(std::string(willLoadPath_));
	ImGui::DragFloat3("Scale", &transform_.scale.x, 0.1f);
	ImGui::DragFloat3("Rotate", &transform_.rotate.x, 0.01f);
	ImGui::DragFloat3("Position", &transform_.position.x, 0.1f);
	ImGui::End();

#endif

	paramManager_->Draw();
	engine_->ImGuiDraw();
}
