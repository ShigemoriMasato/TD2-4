#include "InitializeScene.h"

void InitializeScene::Initialize() {
	auto& windowSet = commonData_->mainWindow;
	windowSet.first = std::make_unique<SHEngine::Screen::WindowsAPI>();
	SHEngine::Screen::WindowsAPI::WindowDesc desc;
	desc.width = 1280;
	desc.height = 720;
	desc.windowName = L"SHEngine";
	desc.wndProc = [&](HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) -> LRESULT {
		return DefWindowProc(hwnd, msg, wparam, lparam);
		};

	windowSet.first->Initialize(desc, engine_->GetHInstance());
	windowSet.second = engine_->MakeWindow(windowSet.first.get(), 0x0000FFFF);

	commonData_->display = std::make_unique<SHEngine::Screen::MultiDisplay>();
	commonData_->display->Initialize(desc.width, desc.height, 0x00FF00FF, textureManager_);

	commonData_->cmdObject = engine_->CreateCommandObject(SHEngine::Command::Type::Direct);
}

std::unique_ptr<IScene> InitializeScene::Update() {
	//更新処理
	commonData_->cmdObject->ResetCommandList();

	return nullptr;
}

void InitializeScene::Draw() {
	auto swapChain = commonData_->mainWindow.second.get();
	auto display = commonData_->display.get();
	auto cmdObj = commonData_->cmdObject.get();

	display->PreDraw(cmdObj, true);
	display->PostDraw(cmdObj);

	swapChain->PreDraw(cmdObj);

	swapChain->PostDraw(cmdObj);
}
