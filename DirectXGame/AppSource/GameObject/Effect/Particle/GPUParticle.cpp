#include "GPUParticle.h"

void GPUParticle::Initialize(SHEngine::Engine* engine)
{
	engine_ = engine;
	for (int i = 0; i < 6; ++i)
	{
		compute_.push_back(engine_->CreateCommandObject(SHEngine::Command::Type::Compute, i));
	}
	direct_ = engine_->CreateCommandObject(SHEngine::Command::Type::Direct);

	pool_ = std::make_unique<GPUParticlePool>();

	compute_[0]->ResetCommandList();
	auto ddManager = engine_->GetDrawDataManager();
	auto modelManager = engine_->GetModelManager();
	auto drawData = ddManager->GetDrawData(modelManager->GetNodeModelData(1).drawDataIndex);
	pool_->Initialize(drawData, compute_[0].get(), 10000);


	// WaitForGPUIdleまでにすべての初期化
	engine_->ExecuteCommand(SHEngine::Command::Type::Compute, 0, { compute_[0].get() });






	// GPUの処理がすべて終わるのを待機する
	compute_[0]->WaitForGPUIdle();
}


void GPUParticle::Update(float dt)
{
	// コマンドリストをリセットして、コマンドを積める状態にする(実行できる状態でなかったら実行できるまで待つ)	
	for (auto& cmdObj : compute_)
	{
		cmdObj->ResetCommandList();
	}
	direct_->ResetCommandList();

	std::vector<SHEngine::Command::WaitFence> waitFences;

	for (int i = 0; i < 6; ++i)
	{
		waitFences.push_back(engine_->ExecuteCommand(SHEngine::Command::Type::Compute, i, { compute_[i].get() }));
	}

	for (int i = 0; i < 6; ++i)
	{
		engine_->WaitFence(waitFences[i], SHEngine::Command::Type::Direct);
	}
}

void GPUParticle::Draw(const Matrix4x4& vp)
{
	pool_->Draw(direct_.get());

	// 描画後にDirectコマンドリストを実行する
	engine_->ExecuteCommand(SHEngine::Command::Type::Direct, 0, { direct_.get() });
}