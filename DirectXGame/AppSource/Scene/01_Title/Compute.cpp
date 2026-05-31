#include "Compute.h"

void Compute::Initialize(SHEngine::Engine* engine) {
	engine_ = engine;
	compute_ = engine->CreateCommandObject(SHEngine::Command::Type::Compute);
	direct_ = engine->CreateCommandObject(SHEngine::Command::Type::Direct);

	compute_->ResetCommandList();

	titleLogo_ = std::make_unique<TitleLogo>();
	titleLogo_->Initialize(engine, compute_.get());

	engine_->ExecuteCommand(SHEngine::Command::Type::Compute, { compute_.get() });
}

void Compute::Update(float deltaTime, Camera* camera) {
	compute_->ResetCommandList();
	direct_->ResetCommandList();

	titleLogo_->Update(deltaTime, camera, compute_.get());

	auto fence = engine_->ExecuteCommand(SHEngine::Command::Type::Compute, { compute_.get() });
	engine_->WaitFence(fence, SHEngine::Command::Type::Direct);
}

void Compute::Draw(SHEngine::Screen::IDisplay* disp) {

	direct_->SetRenderTarget(disp, true);

	titleLogo_->Draw(direct_.get());

	engine_->ExecuteCommand(SHEngine::Command::Type::Direct, { direct_.get() });
}
