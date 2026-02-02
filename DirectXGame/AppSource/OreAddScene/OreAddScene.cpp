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

	fade_.color = { 0.0f, 0.0f, 0.0f };
	fade_.alpha = 1.0f;
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
		if(Wait(deltaTime, 1.0f)) {
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
		if(Wait(deltaTime, 0.5f)) {
			return std::make_unique<GameScene>();
		}
	}

	fade_.alpha = 1.0f - (fadeTimer_ / kFadeDuration_);

	oreNum_->Update(deltaTime);
	goldNum_->Update(deltaTime);

	return std::unique_ptr<IScene>();
}

void OreAddScene::Draw() {

	GameWindow* window = commonData_->mainWindow.get();
	Window* wind = window->GetWindow();
	DualDisplay* display = commonData_->display.get();

	display->PreDraw(window->GetCommandObject(), true);

	Matrix4x4 vpMat = camera_->GetVPMatrix();

	//描画処理
	result_->Draw(wind, vpMat);
	oreNumIs_->Draw(wind, vpMat);
	oreNum_->Draw(wind, vpMat);
	slashNumOre_->Draw(wind, vpMat);
	goldNumIs_->Draw(wind, vpMat);
	goldNum_->Draw(wind, vpMat);
	slashNumGold_->Draw(wind, vpMat);

	display->PostDraw(window->GetCommandObject());

	//PostEffect
#ifdef SH_RELEASE
	postEffectConfig_.output = commonData_->mainWindow->GetDualDisplay();
#endif
	postEffect_->CopyBuffer(PostEffectJob::Fade, fade_);
	postEffect_->CopyBuffer(PostEffectJob::Blur, blur_);
	postEffect_->Draw(postEffectConfig_);

	//もしSwapChainに直接書き込むならココ
	window->PreDraw(true);


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

	if (t >= 1.0f) {
		commonData_->goldNum = commonData_->norma;
		commonData_->oreNum = oreNum;
		goldNum_->SetNumber(commonData_->goldNum);
		oreNum_->SetNumber(commonData_->oreNum);
		t = 0.0f;
		return true;
	}

	return false;
}
