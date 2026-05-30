#include "IntroScene.h"

void IntroScene::Initialize() {
	camera_.Initialize(input_);
	grid_.Initialize(drawDataManager_);

	compute_ = std::make_unique<Compute>();
	compute_->Initialize(engine_);
}

std::unique_ptr<IScene> IntroScene::Update() {
	float deltaTime = engine_->GetDeltaTime();

	camera_.Update();
	compute_->Update(deltaTime, &camera_);

	grid_.Update(camera_.GetCenter(), camera_.GetVPMatrix());



	return std::unique_ptr<IScene>();
}

void IntroScene::Draw() {
	auto display = commonData_->display.get();
	auto window = commonData_->mainWindow.second.get();
	auto cmdobj = commonData_->cmdObject.get();

	compute_->Draw(display->GetDisplay());

	display->PreDraw(cmdobj, false);

	grid_.Draw(cmdobj);
	
	display->PostDraw(cmdobj);



	display->DrawImGui();



	window->PreDraw(cmdobj, true);
	engine_->DrawImGui();
	window->ToPresent(cmdobj);
}
