#include "SHEngine.h"
#include <Tool/Dump/CreateDump.h>
#include <Screen/DualDisplay.h>
#include <Render/RenderObject.h>

#pragma comment(lib, "Dbghelp.lib")

static LONG WINAPI ClashHandler(EXCEPTION_POINTERS* pExceptionPointers) {
	CreateDump(pExceptionPointers);
	return EXCEPTION_EXECUTE_HANDLER;
}

SHEngine::SHEngine() {

	logger_ = getLogger("Engine");

	//Initializeしても変わらない処理

	SetUnhandledExceptionFilter(ClashHandler);

	dxDevice_ = std::make_unique<DXDevice>();
	dxDevice_->Initialize();

	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	assert(SUCCEEDED(hr) && "SHEngine::SHEngine: Failed to initialize COM library");

	cmdListManager_ = std::make_unique<CmdListManager>();
	cmdListManager_->Initialize(dxDevice_.get());

	textureManager_ = std::make_unique<TextureManager>();
	textureManager_->Initialize(dxDevice_.get(), cmdListManager_.get());

	Display::StaticInitialize(dxDevice_.get());
	DualDisplay::StaticInitialize(dxDevice_.get());
	RenderObject::StaticInitialize(dxDevice_.get());

	fenceManager_ = std::make_unique<FenceManager>();
	fenceManager_->Initialize(cmdListManager_->GetCommandQueue(), dxDevice_->GetDevice());

	windowMaker_ = std::make_unique<WindowMaker>();
	windowMaker_->Initialize(dxDevice_.get(), textureManager_.get(), cmdListManager_.get());

	imGuiForEngine_ = std::make_unique<ImGuiforEngine>();

	drawDataManager_ = std::make_unique<DrawDataManager>();
}

SHEngine::~SHEngine() {
	fenceManager_->Finalize();
	if (imGuiActive_) {
		imGuiForEngine_->Finalize();
	}
}

void SHEngine::Initialize() {
	//初期化処理
	drawDataManager_->Initialize(dxDevice_.get());
}

bool SHEngine::IsLoop() {
	return !exit_;
}

void SHEngine::Update() {
	//todo inputとか

	//ImGui
	if (imGuiActive_) {
		imGuiForEngine_->BeginFrame();
		imguiDrawed_ = false;
	}
}

void SHEngine::PreDraw() {
	//Wait
	fenceManager_->WaitForSignal();
	cmdListManager_->Reset();
	auto cmdObj = cmdListManager_->CreateCommandObject();
	textureManager_->UploadTextures(cmdObj->GetCommandList());
}

void SHEngine::EndFrame() {
	//PostDraw
	windowMaker_->AllPostDraw();
	//ImGui
	ImGuiDraw();
	//Execute
	cmdListManager_->Execute();
	//Present
	windowMaker_->PresentAllWindows();

	fenceManager_->SendSignal();

	ExecuteMessage();
	//バツを押されたら終了する
	exit_ = pushedCrossButton_;
}

void SHEngine::ImGuiActivate(Window* window) {
	imGuiForEngine_->Initialize(dxDevice_.get(), cmdListManager_.get(), window);
	imGuiForEngine_->BeginFrame();
	imGuiActive_ = true;

	logger_->info("ImGui activated");
}

void SHEngine::ImGuiDraw() {
	//ImGui
	if (imGuiActive_ && !imguiDrawed_) {
		imGuiForEngine_->EndFrame();
		imguiDrawed_ = true;
	}
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
			return;
		}
	}

	pushedCrossButton_ = true;
}
