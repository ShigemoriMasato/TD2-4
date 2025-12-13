#include "Window.h"

CommandObject* Window::cmdObject_ = nullptr;

void Window::Initialize(DXDevice* device, TextureManager* textureManager, ID3D12CommandQueue* commandQueue, const WindowConfig& config, uint32_t clearColor) {
	//Windowの作成
	windowApp_ = std::make_unique<WindowsApp>();
	windowApp_->SetWindowName(config.windowName);
	windowApp_->SetWindowClassName(config.windowClassName);
	windowApp_->SetSize(config.clientWidth, config.clientHeight);
	windowApp_->SetWindowProc(config.windowProc);
	windowApp_->Create();

	//SwapChainの作成
	swapChain_ = std::make_unique<SwapChain>();
	swapChain_->Initialize(device, textureManager, commandQueue, windowApp_.get(), clearColor);
}

void Window::PreDraw(bool isClear) {
	swapChain_->PreDraw(cmdObject_->GetCommandList(), isClear);
}

void Window::ToTexture() {
	swapChain_->ToTexture(cmdObject_->GetCommandList());
}

void Window::PostDraw() {
	swapChain_->Present();
}
