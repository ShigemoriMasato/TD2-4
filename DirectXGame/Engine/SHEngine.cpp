#include "SHEngine.h"
#include <Tool/Dump/CreateDump.h>

#pragma comment(lib, "Dbghelp.lib")

static LONG WINAPI ClashHandler(EXCEPTION_POINTERS* pExceptionPointers) {
	LogSystem::LogFlush();
	CreateDump(pExceptionPointers);
	return EXCEPTION_EXECUTE_HANDLER;
}

SHEngine::SHEngine() {
	//Initializeしても変わらない処理

	SetUnhandledExceptionFilter(ClashHandler);

	dxDevice_ = std::make_unique<DXDevice>();
	dxDevice_->Initialize();

	cmdListManager_ = std::make_unique<CmdListManager>();
	cmdListManager_->Initialize(dxDevice_.get());

	textureManager_ = std::make_unique<TextureManager>();
	textureManager_->Initialize(dxDevice_.get(), cmdListManager_.get());

	Display::StaticInitialize(dxDevice_.get());

	fenceManager_ = std::make_unique<FenceManager>();
	fenceManager_->Initialize(cmdListManager_->GetCommandQueue(), dxDevice_->GetDevice());
}

SHEngine::~SHEngine() {
	fenceManager_->Finalize();
}

void SHEngine::Initialize() {
	//初期化処理
	textureManager_->Clear();
}

bool SHEngine::IsLoop() {
	return !exit_;
}

void SHEngine::Update() {
	//todo inputとか
}

void SHEngine::PreDraw() {
	//Wait
	fenceManager_->WaitForSignal(0);
	cmdListManager_->Reset();
}

void SHEngine::EndFrame() {
	//Execute
	cmdListManager_->Execute();

	for (const auto& window : windows_) {
		window->Present();
	}

	fenceManager_->SendSignal();

	ExecuteMessage();
	//バツを押されたら終了する
	exit_ = pushedCrossButton_;
}

std::unique_ptr<Window> SHEngine::MakeWindow(const WindowConfig& config, uint32_t clearColor) {
	std::unique_ptr<Window> window = std::make_unique<Window>();
	window->Initialize(dxDevice_.get(), textureManager_.get(), cmdListManager_.get(), config, clearColor);
	windows_.push_back(window.get());
	return std::move(window);
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
