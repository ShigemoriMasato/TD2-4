#include "Window.h"

void Window::Initialize(DXDevice* device, TextureManager* textureManager, CmdListManager* cmdListManager, const WindowConfig& config, uint32_t clearColor) {
	//Windowの作成
	windowApp_ = std::make_unique<WindowsApp>();
	windowApp_->SetWindowName(config.windowName);
	windowApp_->SetWindowClassName(config.windowClassName);
	windowApp_->SetSize(config.clientWidth, config.clientHeight);
	windowApp_->SetWindowProc(config.windowProc);
	windowApp_->Create();
	windowApp_->Show();

	//SwapChainの作成
	swapChain_ = std::make_unique<SwapChain>();
	swapChain_->Initialize(device, textureManager, cmdListManager->GetCommandQueue(), windowApp_.get(), clearColor);

	cmdObject_ = cmdListManager->CreateCommandObject();

	srvManager_ = device->GetSRVManager();
	isPostDraw_ = true;
}

void Window::PreDraw(bool isClear) {
	auto cmdList = cmdObject_->GetCommandList();
	swapChain_->PreDraw(cmdList, isClear);
	
	ID3D12DescriptorHeap* heap[] = { srvManager_->GetHeap() };
	cmdList->SetDescriptorHeaps(1, heap);

	isPostDraw_ = false;
}

void Window::ToTexture() {
	swapChain_->ToTexture(cmdObject_->GetCommandList());
	isPostDraw_ = false;
}

void Window::PostDraw() {
	if(isPostDraw_) {
		return;
	}

	swapChain_->PostDraw(cmdObject_->GetCommandList());
	isPostDraw_ = true;
}

void Window::Present() {
	swapChain_->Present();
}
