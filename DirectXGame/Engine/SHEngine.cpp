#include "SHEngine.h"
#include <Tool/Dump/CreateDump.h>

#pragma comment(lib, "Dbghelp.lib")

static LONG WINAPI ClashHandler(EXCEPTION_POINTERS* pExceptionPointers) {
	LogSystem::LogFlush();
	CreateDump(pExceptionPointers);
	return EXCEPTION_EXECUTE_HANDLER;
}

SHEngine::SHEngine() {
	SetUnhandledExceptionFilter(ClashHandler);
}

SHEngine::~SHEngine() {
	
}

void SHEngine::Initialize() {
	dxDevice_ = std::make_unique<DXDevice>();
	dxDevice_->Initialize();

	cmdListManager_ = std::make_unique<CmdListManager>();
	cmdListManager_->Initialize(dxDevice_.get());

	textureManager_ = std::make_unique<TextureManager>();
	textureManager_->Initialize(dxDevice_.get());
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

std::unique_ptr<Window> SHEngine::MakeWindow(const WindowConfig& config, uint32_t clearColor) {
	return std::unique_ptr<Window>();
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
