#include "IntroScene.h"
#include "TitleScene.h"

IntroScene::~IntroScene() {
	Save();
}

void IntroScene::Initialize() {
	camera_.Initialize(input_);
	grid_.Initialize(drawDataManager_);

	compute_ = std::make_unique<Compute>();
	compute_->Initialize(engine_);

	lightManager_.Initialize();
	gate_.Initialize(engine_, lightManager_.GetLightData());

	multiParticle1_.Initialize(textureManager_, modelManager_, commonData_);
	multiParticle1_.Add("fire.json");
	multiParticle1_.SetEmittingFlag(true);

	multiParticle2_.Initialize(textureManager_, modelManager_, commonData_);
	multiParticle2_.Add("fire.json");
	multiParticle2_.SetEmittingFlag(true);

	auto drawData = drawDataManager_->GetDrawData(commonData_->postEffectDrawDataIndex);
	postEffect_.Initialize(textureManager_, drawData, false);
	postEffectConfig_.cmdObj = commonData_->cmdObject.get();
	postEffectConfig_.jobs_ = (uint32_t)PostEffectJob::Fade;
	postEffectConfig_.origin = commonData_->display->GetDisplay();

	Load();

	fade_.color = { 0.0f, 0.0f, 0.0f };
}

std::unique_ptr<IScene> IntroScene::Update() {
	float deltaTime = engine_->GetDeltaTime();
	auto keys = commonData_->keyManager->GetKeyStates();

	camera_.Update();
	compute_->Update(deltaTime, &camera_);

	grid_.Update(camera_.GetCenter(), camera_.GetVPMatrix());

	gate_.Update(deltaTime, &camera_);

	multiParticle1_.SetModelWorld(particleTrans1_.GetMatrix());
	multiParticle1_.SetCameraPos(camera_.GetPosition());
	multiParticle1_.Update(deltaTime);
	
	multiParticle2_.SetModelWorld(particleTrans2_.GetMatrix());
	multiParticle2_.SetCameraPos(camera_.GetPosition());
	multiParticle2_.Update(deltaTime);

	if (keys[Key::Correct] && !fadeIn_) {
		gate_.Open();
		compute_->GetTitleLogo()->Mist();
		fadeIn_ = true;
	}

	if (keys[Key::Debug1]) {
		compute_->GetTitleLogo()->Default();
		fade_.alpha = 0.0f;
		fadeIn_ = false;
	}


	if (fadeIn_) {
		fade_.alpha += deltaTime * 0.5f;
	}

	if (fade_.alpha > 1.0f) {
		postEffect_.CopyBuffer(PostEffectJob::Fade, fade_);
		return std::make_unique<TitleScene>();
	}

	postEffect_.CopyBuffer(PostEffectJob::Fade, fade_);

	return std::unique_ptr<IScene>();
}

void IntroScene::Draw() {
	auto display = commonData_->display.get();
	auto window = commonData_->mainWindow.second.get();
	auto cmdobj = commonData_->cmdObject.get();

	compute_->Draw(display->GetDisplay());

	display->PreDraw(cmdobj, false);

	//grid_.Draw(cmdobj);
	gate_.Draw(cmdobj);

	multiParticle1_.Draw();
	multiParticle2_.Draw();
	commonData_->particleDrawer->Draw(cmdobj, camera_.GetVPMatrix());
	
	display->PostDraw(cmdobj);

#ifdef USE_IMGUI

	ImGui::Begin("Transform");
	ImGui::DragFloat3("1_Scale", &particleTrans1_.scale.x, 0.01f);
	ImGui::DragFloat3("1_Pos", &particleTrans1_.position.x, 0.01f);

	ImGui::DragFloat3("2_Scale", &particleTrans2_.scale.x, 0.01f);
	ImGui::DragFloat3("2_Pos", &particleTrans2_.position.x, 0.01f);
	ImGui::End();

#endif

	display->DrawImGui();
	lightManager_.DrawImGui();

#ifdef USE_IMGUI

	postEffect_.Draw(postEffectConfig_);
	window->PreDraw(cmdobj, true);

#else

	postEffectConfig_.output = window->GetCurrentDisplay();
	postEffect_.Draw(postEffectConfig_);
	window->PreDraw(cmdobj, false);

#endif

	engine_->DrawImGui();
	window->ToPresent(cmdobj);
}

void IntroScene::Save() {
	BinaryManager bin;
	const std::string fileName = "IntroScene.bin";

	bin.Register(&particleTrans1_);
	bin.Register(&particleTrans2_);

	bin.Write(fileName);
}

void IntroScene::Load() {
	BinaryManager bin;
	const std::string fileName = "IntroScene.bin";
	
	if (!bin.Boot(fileName)) {
		return;
	}

	particleTrans1_ = bin.Reverse<Transform>();
	particleTrans2_ = bin.Reverse<Transform>();
}
