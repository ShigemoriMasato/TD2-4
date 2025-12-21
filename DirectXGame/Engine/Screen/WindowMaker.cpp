#include "WindowMaker.h"

void WindowMaker::Initialize(DXDevice* device, TextureManager* textureManager, CmdListManager* cmdListManager) {
	device_ = device;
	textureManager_ = textureManager;
	cmdListManager_ = cmdListManager;
}

std::unique_ptr<Window> WindowMaker::MakeWindow(const WindowConfig& config, uint32_t clearColor) {
	std::unique_ptr<Window> window = std::make_unique<Window>();
	window->Initialize(device_, textureManager_, cmdListManager_, config, clearColor);
	windows_.push_back(window.get());
	return std::move(window);
}

void WindowMaker::AllPostDraw() {
	for (const auto& window : windows_) {
		window->PostDraw();
	}
}

void WindowMaker::PresentAllWindows() {
	for (const auto& window : windows_) {
		window->Present();
	}
}
