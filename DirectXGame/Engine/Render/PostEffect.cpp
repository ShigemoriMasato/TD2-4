#include "PostEffect.h"

uint32_t operator|(PostEffectJob a, PostEffectJob b) {
	return uint32_t(a) | uint32_t(b);
}

uint32_t operator|(uint32_t a, PostEffectJob b) {
	return a | uint32_t(b);
}

uint32_t operator&(uint32_t a, PostEffectJob b) {
	return a & uint32_t(b);
}

uint32_t operator~(PostEffectJob a) {
    return ~uint32_t(a);
}

bool operator<(PostEffectJob a, PostEffectJob b) {
	return uint32_t(a) < uint32_t(b);
}

void PostEffect::Initialize(TextureManager* textureManager, DrawData& drawData) {
	//PostEffect用Displayの初期化
	intermediateDisplay_ = std::make_unique<DualDisplay>();
	int textureIndex = textureManager->CreateWindowTexture(1280, 720, 0xff0000ff);
	int textureIndex2 = textureManager->CreateWindowTexture(1280, 720, 0xff0000ff);
	auto textureData = textureManager->GetTextureData(textureIndex);
	auto textureData2 = textureManager->GetTextureData(textureIndex2);
	intermediateDisplay_->Initialize(textureData, textureData2);

	//RenderObjectの初期化
	auto createPostEffectObject = [&](PostEffectJob job, std::string psPath) {
		auto postEffectObject = std::make_unique<RenderObject>("PostEffect::" + psPath);
		postEffectObject->Initialize();
		postEffectObject->psoConfig_.vs = "PostEffect/PostEffect.VS.hlsl";
		postEffectObject->psoConfig_.ps = "PostEffect/" + psPath + ".PS.hlsl";
		postEffectObject->SetUseTexture(true);
		postEffectObject->SetDrawData(drawData);
		postEffectObject->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "PostEffect::SourceTexture");
		postEffectObject->CreateCBV(512, ShaderType::PIXEL_SHADER, "PostEffect::SourceTextureOffset");
		postEffectObjects_[job] = std::move(postEffectObject);
		};

	createPostEffectObject(PostEffectJob::None, "Simple");
	createPostEffectObject(PostEffectJob::Blur, "Blur");
	createPostEffectObject(PostEffectJob::Fade, "Fade");
	createPostEffectObject(PostEffectJob::Glitch, "Glitch");
	createPostEffectObject(PostEffectJob::GrayScale, "GrayScale");
	createPostEffectObject(PostEffectJob::GridTransition, "GridTransition");
	createPostEffectObject(PostEffectJob::SlowMotion, "SlowMotion");
}

void PostEffect::Draw(PostEffectConfig config) {
	std::pair<IDisplay*, bool> renderTarget = std::make_pair(intermediateDisplay_.get(), false);
	std::pair<IDisplay*, bool> source = config.origin;

	auto cmdList = config.window->GetCommandObject()->GetCommandList();
	while (config.jobs_ != 0) {
		PostEffectJob currentJob = PostEffectJob::None;
		for(int i = 0; i < 32; ++i) {
			uint32_t job = 1 << i;
			if ((config.jobs_ & job) != 0) {
				currentJob = PostEffectJob(job);
				config.jobs_ &= ~job;
				break;
			}
		}

		renderTarget.first->PreDraw(cmdList, true);
		source.first->ToTexture(cmdList);
		int offset = source.first->GetTextureData()->GetOffset();
		postEffectObjects_[currentJob]->CopyBufferData(0, &offset, sizeof(int));
		postEffectObjects_[currentJob]->psoConfig_.isSwapChain = renderTarget.second;
		postEffectObjects_[currentJob]->Draw(config.window);

		std::swap(renderTarget, source);
	}

	if (!config.output.first) {
		config.output = config.origin;
	}

	if (source.first->GetTextureResource() == config.output.first->GetTextureResource()) {
		return;
	}

	config.output.first->PreDraw(cmdList, false);
	source.first->ToTexture(cmdList);
	int offset = source.first->GetTextureData()->GetOffset();
	postEffectObjects_[PostEffectJob::None]->CopyBufferData(0, &offset, sizeof(int));
	postEffectObjects_[PostEffectJob::None]->psoConfig_.isSwapChain = config.output.second;
	postEffectObjects_[PostEffectJob::None]->Draw(config.window);
}
