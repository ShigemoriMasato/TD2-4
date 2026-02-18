#include "SHEngine.h"
#include <Tool/Dump/CreateDump.h>
#include <Render/Screen/IDisplay.h>
#include <Render/RenderObject.h>

#pragma comment(lib, "Dbghelp.lib")

using namespace SHEngine;

static LONG WINAPI ClashHandler(EXCEPTION_POINTERS* pExceptionPointers) {
	Func::CreateDump(pExceptionPointers);
	return EXCEPTION_EXECUTE_HANDLER;
}

SHEngine::Engine::~Engine() {
	imGuiWrapper_->Finalize();
	CoUninitialize();
}

void Engine::Initialize(HINSTANCE hInstance) {
	SetUnhandledExceptionFilter(ClashHandler);

	logger_ = getLogger("Engine");
	logger_->info("Begin Engine Initialize");

	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	device_ = std::make_unique<DXDevice>();
	device_->Initialize();

	cmdManager_ = std::make_unique<Command::Manager>();
	cmdManager_->Initialize(device_.get());

	imGuiWrapper_ = std::make_unique<ImGuiWrapper>();

	textureManager_ = std::make_unique<TextureManager>();
	textureManager_->Initialize(device_.get(), cmdManager_.get());

	fontLoader_ = std::make_unique<FontLoader>();
	fontLoader_->Initialize(textureManager_.get());

	drawDataManager_ = std::make_unique<DrawDataManager>();
	drawDataManager_->Initialize(device_.get());

	modelManager_ = std::make_unique<ModelManager>();
	modelManager_->Initialize(textureManager_.get(), drawDataManager_.get());

	input_ = std::make_unique<Input>();
	input_->Initialize(hInstance);

	Screen::IDisplay::SetDevice(device_.get());
	RenderObject::StaticInitialize(device_.get());

	hInstance_ = hInstance;
}

bool Engine::IsLoop() {
	if (PeekMessage(&msg_, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg_);
		DispatchMessage(&msg_);
	}
	return msg_.message != WM_QUIT;
}

void Engine::BeginFrame() {
	input_->Update();
}

void Engine::PostDraw() {
	cmdManager_->Execute(Command::Type::Direct);
}

void Engine::EndFrame() {
	cmdManager_->SendSignal(Command::Type::Direct);
}

std::unique_ptr<Screen::SwapChain> SHEngine::Engine::MakeWindow(Screen::WindowsAPI* windowsApi, uint32_t clearColor) {
	auto swapChain = std::make_unique<Screen::SwapChain>();
	swapChain->Initialize(device_.get(), textureManager_.get(), cmdManager_.get(), windowsApi, clearColor);
	return std::move(swapChain);
}
