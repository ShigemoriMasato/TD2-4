#include "SHEngine.h"

SHEngine::SHEngine() {
	dxDevice_ = std::make_unique<DXDevice>();
	dxDevice_->Initialize();

	srvManager_ = std::make_unique<SRVManager>(dxDevice_.get(), 2048);
	rtvManager_ = std::make_unique<RTVManager>(dxDevice_.get(), 128);
	dsvManager_ = std::make_unique<DSVManager>(dxDevice_.get(), 128);

	cmdListManager_ = std::make_unique<CmdListManager>();
	cmdListManager_->Initialize(dxDevice_.get());

	textureManager_ = std::make_unique<TextureManager>();
}

SHEngine::~SHEngine() {
	
}

void SHEngine::Initialize() {
	textureManager_->Initialize(dxDevice_.get(), srvManager_.get());
}

bool SHEngine::IsLoop() {
	return !exit_;
}

void SHEngine::Update() {
	//todo inputとか
}

void SHEngine::PreDraw() {
	//Wait
	cmdListManager_->Reset();
	textureManager_->UploadTextures(cmdListManager_->GetCommandObject(CmdListType::Texture)->GetCommandList());
}

void SHEngine::EndFrame() {
	ExecuteMessage();
	//バツを押されたら終了する
	exit_ = pushedCrossButton_;
}

void SHEngine::ExecuteMessage() {
	pushedCrossButton_ = false;

	while (msg_.message != WM_QUIT) {
		//メッセージがあれば処理する
		if (PeekMessage(&msg_, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg_);
			DispatchMessage(&msg_);
		} else {
			//メッセージがなければ処理を始める
			pushedCrossButton_ = false;
		}
	}

	pushedCrossButton_ = true;
}
