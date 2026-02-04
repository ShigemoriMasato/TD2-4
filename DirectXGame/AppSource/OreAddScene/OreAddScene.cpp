#include "OreAddScene.h"
#include <Utility/Easing.h>
#include <Game/GameScene.h>

namespace {
	std::string fontName = "YDWbananaslipplus.otf";
}

void OreAddScene::Initialize() {
	camera_ = std::make_unique<Camera>();
	camera_->SetProjectionMatrix(OrthographicDesc{});
	camera_->MakeMatrix();

	DrawData drawData = drawDataManager_->GetDrawData(1);

	result_ = std::make_unique<FontObject>();
	int stageNum = commonData_->nextMapIndex - 1;
	if (commonData_->isEndlessMode) {
		stageNum = commonData_->stageCount;
	}
	result_->Initialize(fontName, L"Floor" + std::to_wstring(stageNum) + L" Result", drawData, fontLoader_);
	result_->transform_.scale = { 1.5f, -1.6f, 1.0f };
	result_->transform_.position = { 340.0f, 189.0f, 0.0f };
	result_->fontColor_ = ConvertColor(0xffffffff);

	goldNumIs_ = std::make_unique<FontObject>();
	goldNumIs_->Initialize(fontName, L"Gold: ", drawData, fontLoader_);
	goldNumIs_->transform_.scale = { 1.0f, -1.0f, 0.0f };
	goldNumIs_->transform_.position = { 341.0f, 360.0f, 0.0f };
	goldNumIs_->fontColor_ = ConvertColor(0xcfb87bff);

	goldNum_ = std::make_unique<Number>();
	goldNum_->Initialize(fontName, drawData, fontLoader_);
	goldNum_->transform_.position = { 700.0f, 360.0f, 0.0f };
	goldNum_->SetColor(0xcfb87bff, 0xeeeeffff);
	goldNum_->SetNumber(commonData_->goldNum);

	slashNumGold_ = std::make_unique<FontObject>();
	slashNumGold_->Initialize(fontName, L"/ " + std::to_wstring(commonData_->norma), drawData, fontLoader_);
	slashNumGold_->transform_.scale = { 1.0f, -1.0f, 0.0f };
	slashNumGold_->transform_.position = { 800.0f, 360.0f, 0.0f };
	slashNumGold_->fontColor_ = ConvertColor(0xcfb87bff);

	oreNumIs_ = std::make_unique<FontObject>();
	oreNumIs_->Initialize(fontName, L"Ore: ", drawData, fontLoader_);
	oreNumIs_->transform_.scale = { 1.0f, -1.0f, 0.0f };
	oreNumIs_->transform_.position = { 367.0f, 522.0f, 0.0f };
	oreNumIs_->fontColor_ = ConvertColor(0xc2d283ff);

	oreNum_ = std::make_unique<Number>();
	oreNum_->Initialize(fontName, drawData, fontLoader_);
	oreNum_->transform_.position = { 700.0f, 522.0f, 0.0f };
	oreNum_->SetColor(0xc2d283ff, 0xeeeeffff);
	oreNum_->SetNumber(commonData_->oreNum);

	slashNumOre_ = std::make_unique<FontObject>();
	slashNumOre_->Initialize(fontName, L"/ " + std::to_wstring(commonData_->oreNum), drawData, fontLoader_);
	slashNumOre_->transform_.scale = { 1.0f, -1.0f, 0.0f };
	slashNumOre_->transform_.position = { 800.0f, 522.0f, 0.0f };
	slashNumOre_->fontColor_ = ConvertColor(0xc2d283ff);

	postEffect_ = std::make_unique<PostEffect>();
	postEffect_->Initialize(textureManager_, drawDataManager_->GetDrawData(commonData_->postEffectDrawDataIndex));
	postEffectConfig_.jobs_ = UINT(PostEffectJob::Fade);
	postEffectConfig_.window = commonData_->mainWindow->GetWindow();
	postEffectConfig_.origin = commonData_->display.get();

	fukidashi_ = std::make_unique<RenderObject>();
	fukidashi_->SetDrawData(drawData);
	fukidashi_->CreateSRV(sizeof(Matrix4x4) * 2, 2, ShaderType::VERTEX_SHADER, "wvpMat");
	fukidashi_->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "textureIndex");
	fukidashi_->SetUseTexture(true);
	fukidashi_->psoConfig_.vs = "Simples.VS.hlsl";
	fukidashi_->psoConfig_.ps = "PostEffect/Simple.PS.hlsl";
	fukidashi_->psoConfig_.rasterizerID = RasterizerID::Back;
	fukidashi_->instanceNum_ = 2;
	fkdsTextureIndex_ = textureManager_->LoadTexture("fukidashi.png");
	fukidashiTransforms_.resize(2);

	fade_.color = { 0.0f, 0.0f, 0.0f };
	fade_.alpha = 1.0f;

	addNum_ = std::make_unique<Number>();
	addNum_->Initialize(fontName, drawData, fontLoader_);
	addNum_->SetOffset(L"+");
	addNum_->SetColor(0x000000ff, 0xffff30ff);
	addNum_->SetMaxScale(1.4f);


	difficulty_ = std::make_unique<Difficult>();
	bool reset = commonData_->stageCount == 1;
	difficulty_->Initialize(modelManager_, drawDataManager_, fontLoader_, reset);

	endProcess_ = std::make_unique<PostEffect>();
	endProcess_->Initialize(textureManager_, drawDataManager_->GetDrawData(commonData_->postEffectDrawDataIndex));
	endConfig_.jobs_ = 0;
	endConfig_.origin = commonData_->display.get();
	endConfig_.window = commonData_->mainWindow->GetWindow();

	Load();
}

std::unique_ptr<IScene> OreAddScene::Update() {
	input_->Update();

	float deltaTime = engine_->GetFPSObserver()->GetDeltatime();

	if (phase_ == 0) {
		fadeTimer_ += deltaTime;
		if (fadeTimer_ >= kFadeDuration_) {
			fadeTimer_ = kFadeDuration_;
			phase_++;
		}
	} else if (phase_ == 1) {
		if (OreAdd(deltaTime)) {
			phase_++;
		}
	} else if (phase_ == 2) {
		if (Wait(deltaTime, 0.5f)) {
			if (!commonData_->isEndlessMode) {
				phase_ = 10;
			} else {
				phase_++;
			}

		}
	} else if (phase_ == 3) {
		static float t = 0.0f;
		t += deltaTime;
		cameraTransform_.rotate.y = lerp(0.0f, 3.14159265f, t, EaseType::EaseInOutCubic);
		cameraTransform_.rotate.x = std::sin(3.14159265f * t * 2.0f) * 0.2f;
		if (t >= 0.5f) {
			camera_->SetProjectionMatrix(PerspectiveFovDesc());
			notDraw_ = true;
		}
		if (t >= 1.0f) {
			t = 0.0f;
			cameraTransform_.rotate.y = 3.14159265f;
			cameraTransform_.rotate.x = 0.0f;
			phase_++;
		}
	} else if (phase_ == 4) {
		if (Wait(deltaTime, 0.5f)) {
			difficulty_->DifficultyUp(commonData_->stageCount % 2 == 0);
			phase_++;
		}
	} else if (phase_ == 5) {
		if (difficulty_->AnimationEnd()) {
			phase_ = 10;
		}
	}
	//此処で演出入れるならいれる
	else if (phase_ == 10) {
		fadeTimer_ -= deltaTime;
		if (fadeTimer_ <= 0.0f) {
			fadeTimer_ = 0.0f;
			phase_++;
		}
	} else {
		if (Wait(deltaTime, 0.2f)) {
			return std::make_unique<GameScene>();
		}
	}

	camera_->SetTransform(Matrix::MakeAffineMatrix(cameraTransform_.scale, cameraTransform_.rotate, cameraTransform_.position).Inverse());
	camera_->MakeMatrix();
	fade_.alpha = 1.0f - (fadeTimer_ / kFadeDuration_);

	addNum_->SetNumber(oreAddNum_);
	addNum_->SetTransform(addNumTransform_);

	difficulty_->Update(deltaTime);

	oreNum_->Update(deltaTime);
	goldNum_->Update(deltaTime);
	addNum_->Update(deltaTime);

	return std::unique_ptr<IScene>();
}

void OreAddScene::Draw() {

	GameWindow* window = commonData_->mainWindow.get();
	Window* wind = window->GetWindow();
	DualDisplay* display = commonData_->display.get();

	display->PreDraw(window->GetCommandObject(), true);

	Matrix4x4 vpMat = camera_->GetVPMatrix();

	if (!notDraw_) {
		//描画処理
		result_->Draw(wind, vpMat);
		oreNumIs_->Draw(wind, vpMat);
		oreNum_->Draw(wind, vpMat);
		slashNumOre_->Draw(wind, vpMat);
		goldNumIs_->Draw(wind, vpMat);
		goldNum_->Draw(wind, vpMat);
		slashNumGold_->Draw(wind, vpMat);
		Matrix4x4 mat[4];
		for (int i = 0; i < 4; i += 2) {
			mat[i] = Matrix::MakeAffineMatrix(fukidashiTransforms_[i / 2].scale, fukidashiTransforms_[i / 2].rotate, fukidashiTransforms_[i / 2].position);
			mat[i + 1] = vpMat;
		}
		fukidashi_->CopyBufferData(0, mat, sizeof(Matrix4x4) * 4);
		fukidashi_->CopyBufferData(1, &fkdsTextureIndex_, sizeof(int));
		fukidashi_->Draw(wind);
		addNum_->Draw(wind, vpMat);
	}
	difficulty_->Draw(wind, vpMat);

	display->PostDraw(window->GetCommandObject());

	//PostEffect
#ifdef SH_RELEASE
	postEffectConfig_.output = commonData_->mainWindow->GetDualDisplay();
#endif
	postEffect_->CopyBuffer(PostEffectJob::Fade, fade_);
	postEffect_->Draw(postEffectConfig_);

	//もしSwapChainに直接書き込むならココ
	window->PreDraw(false);


#ifdef USE_IMGUI

	//ImGui
	ImGui::Begin("OreAddScene");

	ImGui::DragFloat3("resultPos", &result_->transform_.position.x);
	ImGui::DragFloat3("resultScale", &result_->transform_.scale.x, 0.1f);
	ImGui::ColorEdit4("resultColor", &result_->fontColor_.x);

	ImGui::DragFloat3("oreNumIsPos", &oreNumIs_->transform_.position.x);
	ImGui::DragFloat3("oreNumIsScale", &oreNumIs_->transform_.scale.x, 0.1f);
	ImGui::ColorEdit4("oreNumIsColor", &oreNumIs_->fontColor_.x);

	ImGui::DragFloat3("goldNumIsPos", &goldNumIs_->transform_.position.x);
	ImGui::DragFloat3("goldNumIsScale", &goldNumIs_->transform_.scale.x, 0.1f);
	ImGui::ColorEdit4("goldNumIsColor", &goldNumIs_->fontColor_.x);

	for (int i = 0; i < 1; ++i) {
		ImGui::PushID(i);
		ImGui::DragFloat3("Scale", &fukidashiTransforms_[i].scale.x, 0.1f);
		ImGui::DragFloat3("Rotate", &fukidashiTransforms_[i].rotate.x, 0.1f);
		ImGui::DragFloat3("Position", &fukidashiTransforms_[i].position.x, 1.0f);
		ImGui::PopID();
	}

	ImGui::DragFloat3("addNumScale", &addNumTransform_.scale.x, 0.1f);
	ImGui::DragFloat3("addNumRotate", &addNumTransform_.rotate.x, 0.1f);
	ImGui::DragFloat3("addNumPosition", &addNumTransform_.position.x, 1.0f);

	ImGui::End();

	//ImGuiの最終処理
	window->DrawDisplayWithImGui();
	engine_->ImGuiDraw();
#endif
}

bool OreAddScene::Wait(float deltaTime, float targetTime) {
	static float timer = 0.0f;
	timer += deltaTime;
	if (timer >= targetTime) {
		timer = 0.0f;
		return true;
	}
	return false;
}

bool OreAddScene::OreAdd(float deltaTime) {

	static float t = 0.0f;
	t += deltaTime / 4.0f;

	t = std::min(t, 1.0f);

	int goldNum = lerp(commonData_->goldNum, commonData_->norma, t, EaseType::EaseOutQuint);
	int oreNum = commonData_->oreNum + (commonData_->goldNum - goldNum) / rateGoldToOre_;

	goldNum_->SetNumber(goldNum);
	oreNum_->SetNumber(oreNum);
	oreAddNum_ = oreNum - commonData_->oreNum;

	if (t >= 1.0f) {
		oreAddNum_ = oreNum - commonData_->oreNum;
		commonData_->getOreNum += oreAddNum_;
		commonData_->goldNum = commonData_->norma;
		commonData_->oreNum = oreNum;
		goldNum_->SetNumber(commonData_->goldNum);
		oreNum_->SetNumber(commonData_->oreNum);
		t = 0.0f;
		return true;
	}

	return false;
}

void OreAddScene::Save() {
	for (int i = 0; i < 1; ++i) {
		binaryManager_.RegisterOutput(fukidashiTransforms_[i].scale);
		binaryManager_.RegisterOutput(fukidashiTransforms_[i].rotate);
		binaryManager_.RegisterOutput(fukidashiTransforms_[i].position);
	}
	binaryManager_.RegisterOutput(addNumTransform_.scale);
	binaryManager_.RegisterOutput(addNumTransform_.rotate);
	binaryManager_.RegisterOutput(addNumTransform_.position);
	binaryManager_.Write("OreAdd");
}

void OreAddScene::Load() {
	auto values = binaryManager_.Read("OreAdd");
	int index = 0;
	if (values.empty()) {
		return;
	}
	for (int i = 0; i < 1; ++i) {
		fukidashiTransforms_[i].scale = BinaryManager::Reverse<Vector3>(values[index++].get());
		fukidashiTransforms_[i].rotate = BinaryManager::Reverse<Vector3>(values[index++].get());
		fukidashiTransforms_[i].position = BinaryManager::Reverse<Vector3>(values[index++].get());
	}
	addNumTransform_.scale = binaryManager_.Reverse<Vector3>(values[index++].get());
	addNumTransform_.rotate = binaryManager_.Reverse<Vector3>(values[index++].get());
	addNumTransform_.position = binaryManager_.Reverse<Vector3>(values[index++].get());
}
